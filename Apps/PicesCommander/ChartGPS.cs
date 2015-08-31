using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Threading;
using System.IO;

using SipperFile;
using PicesInterface;

namespace PicesCommander
{
  public partial class ChartGPS : Form
  {
    private  String             cruiseName    = null;
    private  PicesSipperCruise  cruise        = null;
    private  String             stationName   = null;
    private  PicesSipperStation station       = null;
    private  String             deploymentNum = null;
    private  int                timeInterval  = 10;   /**< Interval in Seconds between GPS datapoints to  plot. */

    private  PicesDataBase      mainWinConn   = null;
    private  PicesDataBase      threadConn    = null;  /**< This connection will only be use by the background thread. */

    private  PicesMsgQueue      statusMsgs    = null;
    private  PicesMsgQueue      msgQueue      = null;
    private  PicesRunLog        runLog        = null;

    private  PicesGoalKeeper  goalie = new PicesGoalKeeper ("ChartGPS");

    private  bool  buildPlotDataRunning = false;
    private  bool  buildPlotDataDone    = false;
    private  bool  cancelRequested      = false;

    private  int  heightLast = 0;
    private  int  widthLast  = 0;

    private  Color[]  colorTable = {Color.Black,      Color.Red,        Color.Green,      
                                    Color.Blue,       Color.Crimson,    Color.DarkCyan,   
                                    Color.DarkOrange, Color.Lime,       Color.SeaGreen
                                   };

    private  String  configFileName = "";
    private  bool    formIsMaximized = false;

    private  double  degToRad = Math.PI / 180.0;
    private  double  radToDeg = 180.0   / Math.PI;

    private  TimeSpan  cruiseAdjGpsToActTime = new TimeSpan (0, 0, 0);

    private  PicesSipperStationList  stations = null;


    
    private  class  PlotRequest
    {
      public  PlotRequest (PicesSipperDeployment  _deployment, 
                           Color                  _color
                          )
      {
        deployment = _deployment;
        color      = _color;
      }

      public  PicesSipperDeployment  deployment  = null;        // Class or Instrument name.
      public  Color                  color       = Color.Black;
    }


    List<PlotRequest>  plotRequests = null;

    bool               plotCruise = false;

    /// <summary>
    /// Data for one series that will be plotted will be maintained in a single instance of this class.
    /// There will be one instance for each series.
    /// </summary>
    private  class  DataSeriesToPlot
    {
      public  DataSeriesToPlot (PicesSipperCruise      _cruise,
                                PicesGPSDataPointList  _data
                               ) 
      {
        cruise = _cruise;
        data   = _data;
      }


      public  DataSeriesToPlot (PicesSipperDeployment  _deployment,
                                PicesGPSDataPointList  _data
                               ) 
      {
        deployment = _deployment;
        data       = _data;
      }


      public  void  LocateGPSBounds ()
      {
        foreach  (PicesGPSDataPoint  dp in data)
        {
          if  (dp.Latitude > latitudeMax)
            latitudeMax = dp.Latitude;
          if  (dp.Latitude < latitudeMin)
            latitudeMin = dp.Latitude;

          if  (dp.Longitude > longitudeMax)
            longitudeMax = dp.Longitude;
          if  (dp.Longitude < longitudeMin)
            longitudeMin = dp.Longitude;

          if (dp.GpsUtcTime > gpsDateTimeEnd)
            gpsDateTimeEnd = dp.GpsUtcTime;

          if (dp.GpsUtcTime < gpsDateTimeStart)
            gpsDateTimeStart = dp.GpsUtcTime;
        }
      }



      /** Returns true if this series Min/Max place it with in the threshold area of the specified point. */
      public  bool  WithInThreshold (double  longitude,  double longTh,
                                     double  latitude,   double latTh
                                    )
      {
        return  (((longitude + longTh) > longitudeMin)   &&  ((longitude - longTh) < longitudeMax)  &&
                 ((latitude  + latTh)  > latitudeMin)    &&  ((latitude  - latTh)  < latitudeMax)
                );
      }



      public  PicesGPSDataPoint ClosestPoint (double  longitude,  double longTh,
                                              double  latitude,   double latTh,
                                              ref int closestPointIndex,
                                              ref double  closestPointSquareDist
                                             )
      {
        PicesGPSDataPoint   closestPoint = null;
        closestPointSquareDist = double.MaxValue;
        closestPointIndex = -1;

        double longitudeMin = longitude - longTh;
        double longitudeMax = longitude + longTh;
        double latitudeMin  = latitude  - longTh;
        double latitudeMax  = latitude  + longTh;

        int  index = 0;
        foreach  (PicesGPSDataPoint dp in data)
        {
          if  ((dp.Longitude >= longitudeMin)  &&  (dp.Longitude <= longitudeMax))
          {
            if  ((dp.Latitude >= latitudeMin)  &&  (dp.Latitude <= latitudeMax))
            {
              double  deltaLatitude  = (dp.Latitude  - latitude)  / latTh;
              double  deltaLongitude = (dp.Longitude - longitude) / longTh;

              double  squareDist = deltaLatitude * deltaLatitude + deltaLongitude * deltaLongitude;
              if  (squareDist < closestPointSquareDist)
              {
                closestPoint = dp;
                closestPointSquareDist = squareDist;
                closestPointIndex = index;
              }
            }
          }
          index++;
        }
        return  closestPoint;
      }  /* ClosestPoint */

      /// <summary>
      ///  cruise and deployment are mutually exclusive.  The data in this series is for either a 
      ///  single deployment or an entire cruise.  If it is for an entire cruise it will only
      ///  contain data for the time span covers all the deployments being plotted.
      /// </summary>

      public  PicesSipperCruise      cruise            = null;
      public  PicesSipperDeployment  deployment        = null; 
      public  PicesGPSDataPointList  data              = null;
      public  double                 latitudeMin       = double.MaxValue;
      public  double                 latitudeMax       = double.MinValue;
      public  double                 longitudeMin      = double.MaxValue;
      public  double                 longitudeMax      = double.MinValue;
      public  DateTime               gpsDateTimeStart  = DateTime.MaxValue;
      public  DateTime               gpsDateTimeEnd    = DateTime.MinValue;
      public  int                    chartSeriesIndex  = -1;
    }  /* DataSeriesToPlot */

    

    private  List<DataSeriesToPlot>  series = new List<DataSeriesToPlot> ();


    public ChartGPS (String  _cruiseName,
                     String  _stationName,
                     String  _deploymentNum
                    )
    {
      cruiseName    = _cruiseName;
      stationName   = _stationName;
      deploymentNum = _deploymentNum;

      statusMsgs = new PicesMsgQueue ("ChartGPS-StatusMsgs");
      msgQueue   = new PicesMsgQueue ("ChartGPS-RunLog");
      runLog     = new PicesRunLog (msgQueue);

      mainWinConn = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);

      configFileName = OSservices.AddSlash (PicesSipperVariables.ConfigurationDirectory ()) + "ChartGPS.cfg";

      InitializeComponent ();
    }

   
    private void  ChartGPS_Load (object sender, EventArgs e)
    {
      heightLast = Height;
      widthLast  = Width;

      DeploymentsToPlot.Items.Clear ();
      
      Cruise.Text     = cruiseName;
      Station.Text    = stationName;
      Deployment.Text = deploymentNum;

      cruise  = mainWinConn.SipperCruiseLoad (cruiseName);
      station = mainWinConn.SipperStationLoad (cruiseName, stationName);

      ContextMenuStrip cms = new ContextMenuStrip ();
      cms.Items.Add ("Copy Chart to clipboard",               null, CopyChartToClipboard);
      cms.Items.Add ("Save Chart to Disk",                    null, SaveChartToDisk);
      cms.Items.Add ("Copy Data Tab-Delimited to Clipboard",  null, SaveTabDelToClipBoard);
      cms.Items.Add ("Save Data Tab-Delimited to Disk",       null, SaveTabDelToDisk);

      ProfileChart.ContextMenuStrip = cms;

      LoadConfigurationFile ();

      PopulatDeploymentsToPlot ();

      PlotCruiseField.Checked = plotCruise;

      PlotButton_Click (this, null);
    }



    private  void  PopulatDeploymentsToPlot ()
    {
      DeploymentsToPlot.Items.Clear ();
      PicesSipperDeploymentList  deployments = mainWinConn.SipperDeploymentLoad (cruiseName, "");
      if  (deployments == null)
        return;

      stations = mainWinConn.SipperStationLoad (cruiseName);

      cruiseAdjGpsToActTime = new TimeSpan (0, 0, 0);
      long  adjGpsToActTime = 0;

      foreach  (PicesSipperDeployment  deployment in deployments)
      {
        long  deploymentAdjGpsToAct = deployment.SyncTimeStampGPS.ToFileTimeUtc () - deployment.SyncTimeStampActual.ToFileTimeUtc ();
        if  (deploymentAdjGpsToAct != adjGpsToActTime)
        {
          adjGpsToActTime = deploymentAdjGpsToAct;
          cruiseAdjGpsToActTime = deployment.SyncTimeStampActual - deployment.SyncTimeStampGPS;
        }

        DeploymentsToPlot.Items.Add (deployment, true);
      }
    }



    private  PicesSipperDeploymentList  CurrentlySelectedDeployments ()
    {
      PicesSipperDeploymentList  deployments = new PicesSipperDeploymentList ();

      foreach  (Object so in DeploymentsToPlot.CheckedItems)
        deployments.Add ((PicesSipperDeployment)so);

      return  deployments;
    }



    private  PicesSipperDeployment  CurrentlySelectedDeployment ()
    {
      PicesSipperDeploymentList  deployments = CurrentlySelectedDeployments ();
      if  ((deployments == null)  ||  (deployments.Count < 1))
        return null;
      else
        return  deployments[0];
    }



    private  void  DisableControls ()
    {
      TimeInterval.Enabled      = false;
      PlotButton.Enabled        = false;
      DeploymentsToPlot.Enabled = false;
      TimeInterval.Enabled      = false;
      PlotCruiseField.Enabled   = false;
    }


    private  void  EnableControls ()
    {
      TimeInterval.Enabled      = true;
      PlotButton.Enabled        = true;
      DeploymentsToPlot.Enabled = true;
      TimeInterval.Enabled      = true;
      PlotCruiseField.Enabled   = true;
    }


    private  void  SaveConfiguration ()
    {
      System.IO.StreamWriter  o = null;
      try{o = new System.IO.StreamWriter (configFileName);}  catch  (Exception){o = null; return;}
      
      o.WriteLine ("//ChartGPS Configuration File");
      o.WriteLine ("//");
      o.WriteLine ("//DateTime Saved" + "\t" + DateTime.Now.ToString ("F"));
      o.WriteLine ("//");
      o.WriteLine ("WidthLast"     + "\t" + widthLast);
      o.WriteLine ("HeightLast"    + "\t" + heightLast);
      o.WriteLine ("Maximized"     + "\t" + (formIsMaximized ? "YES":"NO"));
      o.WriteLine ("TimeInterval"  + "\t" + TimeInterval.Value);
      o.WriteLine ("PlotCruise"    + "\t" + (plotCruise ? "True" : "False"));

      o.Close ();
      o = null;
    }


    private  void   LoadConfigurationFile ()
    {
      System.IO.StreamReader i = null;

      try {i = new System.IO.StreamReader (configFileName);}  catch  (Exception) {i = null;}
      if  (i == null)
        return;

      int  savedWidth  = 0;
      int  savedHeight = 0;
      bool screenWasMaximized = false;

      String  nextLine = null;

      while  (true)
      {
        try  {nextLine = i.ReadLine ();}  catch (Exception) {break;}
        if  (nextLine == null)
          break;

        nextLine = nextLine.Trim ();
        
        if  ((nextLine.Length < 3)  ||  (nextLine.Substring (0, 2) == "//"))
          continue;

        String[] fields = nextLine.Split ('\t');
        if  (fields.Length < 2)
          continue;

        String  fieldName  = fields[0];
        String  fieldValue = fields[1];

        switch  (fieldName)
        { 
          case  "WidthLast":
            savedWidth = PicesKKStr.StrToInt (fieldValue);
            break;

          case  "HeightLast":
            savedHeight = PicesKKStr.StrToInt (fieldValue);
            break;

          case  "Maximized":
            screenWasMaximized  = (fieldValue.ToUpper () == "YES");
            break;

          case  "TimeInterval":
            timeInterval = (int)PicesKKStr.StrToFloat (fieldValue);
            if  (timeInterval > 0)
              TimeInterval.Value = (decimal)timeInterval;
            break;

          case  "PlotCruise":
            plotCruise = PicesKKStr.StrToBool (fieldValue);
            break;
        }
      }

      i.Close ();

      if  (savedWidth > Screen.PrimaryScreen.Bounds.Width)
        savedWidth = Screen.PrimaryScreen.Bounds.Width;

      if  (savedHeight > Screen.PrimaryScreen.Bounds.Height)
        savedHeight = Screen.PrimaryScreen.Bounds.Height;

      if  (screenWasMaximized)
      {
        //savedWidth = Screen.PrimaryScreen.Bounds.Width;
        //savedHeight = savedHeight = Screen.PrimaryScreen.Bounds.Height;
        this.WindowState = FormWindowState.Maximized; 
      }
      else
      {
        Height = Math.Max (savedHeight, MinimumSize.Height);
        Width  = Math.Max (savedWidth,  MinimumSize.Width);
      }

      OnResizeEnd (new EventArgs ());
    }  /* LoadConfigurationFile */




    private  List<String>  ValidateFields ()
    {
      List<String>  errors = new List<string> ();

      timeInterval = (int)TimeInterval.Value;
      if  (timeInterval < 1)
        errors.Add ("TimeInterval must be greater than zero.");

      if  (DeploymentsToPlot.CheckedItems.Count < 1)
        errors.Add ("You must have at least one Deployment Checked.");
      
      if  (errors.Count < 1)
        return null;
      else
        return errors;
    }




    private  void  AddDeploymentToSeries (PicesSipperDeployment  deployment,
                                          Color                  color
                                         )
    {
      PicesGPSDataPointList  gpsData = threadConn.InstrumentDataRetrieveGPSInfo (deployment.CruiseName, deployment.StationName, deployment.DeploymentNum, (int)TimeInterval.Value);
      if  ((gpsData == null)  ||  cancelRequested)
        return;

      PicesGPSDataPointList  fliteredGPSData = gpsData.FilterOutNoise ();
      if  (fliteredGPSData == null)
        return;

      if  (fliteredGPSData.Count > 1)
      {
        DataSeriesToPlot  seriesToPlot = new DataSeriesToPlot (deployment, fliteredGPSData);
        seriesToPlot.LocateGPSBounds ();
        goalie.StartBlock ();
        series.Add (seriesToPlot);
        goalie.EndBlock ();
      }
    }  /* AddDeploymentToSeries */




    /// <summary>
    /// Load GPS data that covers the time span of all the deployments that are being plotted.
    /// </summary>
    private  void  AddCruiseToSeries ()
    {
      DateTime  gpsDateTimeStart = DateTime.MaxValue;
      DateTime  gpsDateTimeEnd   = DateTime.MinValue;

      foreach  (DataSeriesToPlot  dsp in series)
      {
        if  (dsp.gpsDateTimeStart < gpsDateTimeStart)
          gpsDateTimeStart = dsp.gpsDateTimeStart;

        if  (dsp.gpsDateTimeEnd > gpsDateTimeEnd)
          gpsDateTimeEnd = dsp.gpsDateTimeEnd;
      }

      int  totalSecs = (int)(gpsDateTimeEnd - gpsDateTimeStart).TotalSeconds;
      int  proposedInterval = totalSecs / 1500;
      if  (proposedInterval < timeInterval)
        proposedInterval = timeInterval;

      PicesGPSDataPointList  gpsData = threadConn.GpsDataQueryByIntervals (cruiseName, gpsDateTimeStart,  gpsDateTimeEnd, proposedInterval);
      if  (gpsData != null)
      { 
        DataSeriesToPlot  seriesToPlot = new DataSeriesToPlot (cruise, gpsData);
        seriesToPlot.LocateGPSBounds ();
        goalie.StartBlock ();
        series.Add (seriesToPlot);
        goalie.EndBlock ();
      }
    }  /* AddCruiseToSeries */




    /// <summary>
    /// This next method will be ran as a separate thread; it is responsible for collecting all the data needed to generate the plot.
    /// </summary>
    private  void  BuildPlotData ()
    {
      if  (buildPlotDataRunning)
        return;

      PicesDataBase.ThreadInit ();

      threadConn = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);

      buildPlotDataRunning = true;

      timeInterval = (int)TimeInterval.Value;

      series = new List<DataSeriesToPlot> ();
      foreach  (PlotRequest pr in plotRequests)
      {
        if  (cancelRequested)
          break;
        AddDeploymentToSeries (pr.deployment, pr.color);
      }

      if  (plotCruise &&  (!cancelRequested))
        AddCruiseToSeries ();

      threadConn.Close ();
      threadConn = null;
      GC.Collect ();
      PicesDataBase.ThreadEnd ();

      if  (cancelRequested)
        statusMsgs.AddMsg ("Plotting of data CANCELLED!!!");
      else
        statusMsgs.AddMsg ("Building of plot data completed !!!");

      buildPlotDataRunning = false;
      buildPlotDataDone = true;
    }  /* BuildPlotData */



    private  void  CancelBackGroundThread ()
    {
      cancelRequested = true;
      if  (threadConn != null)
        threadConn.CancelFlag = true;
      statusMsgs.AddMsg ("Cancel Requested");
    }



    private  void  AddSeriesToChart (DataSeriesToPlot  dataSeries,
                                     ref double        minX,
                                     ref double        maxX,
                                     ref double        minY,
                                     ref double        maxY,
                                     ref DateTime      minGpsDateTime,
                                     ref DateTime      maxGpsDateTime
                                    )
    {
      PicesGPSDataPointList  gpsData    = dataSeries.data;
      PicesSipperDeployment  deployment = dataSeries.deployment;
      PicesSipperCruise      cruise     = dataSeries.cruise;

      if  (gpsData.Count < 2)
        return;

      TimeSpan  adjGpsToActTime = new TimeSpan (0, 0, 0);
      TimeSpan  adjGpsToCTDTime = new TimeSpan (0, 0, 0);

      if  (deployment != null)
      {
        adjGpsToActTime = deployment.SyncTimeStampActual - deployment.SyncTimeStampGPS;
        adjGpsToCTDTime = deployment.SyncTimeStampCTD    - deployment.SyncTimeStampGPS;
      }
      else if  (cruise != null)
      {
        adjGpsToActTime = cruiseAdjGpsToActTime;
        adjGpsToCTDTime = cruiseAdjGpsToActTime;
      }

      DateTime startTimeGps  = new DateTime (1, 1, 1, 1, 1, 1);
      DateTime endTimeGps    = new DateTime (1, 1, 1, 1, 1, 1);
      if  (gpsData.Count > 0)
      {
        startTimeGps = gpsData[0].GpsUtcTime;
        if  (startTimeGps.CompareTo (minGpsDateTime) < 0)
          minGpsDateTime = startTimeGps;
      }

      if  (gpsData.Count > 1)
      {
        endTimeGps = gpsData[gpsData.Count - 1].GpsUtcTime;
        if  (endTimeGps.CompareTo (maxGpsDateTime) > 0)
          maxGpsDateTime = endTimeGps;
      }

      DateTime startTimeAct = startTimeGps.Add (adjGpsToActTime);
      DateTime endTimeAct   = endTimeGps.Add   (adjGpsToActTime);

      String startTimeStr = startTimeAct.Hour.ToString("00") + ":" + startTimeAct.Minute.ToString("00");
      String endTimeStr   = endTimeAct.Hour.ToString("00")   + ":" + endTimeAct.Minute.ToString("00");

      String  legend = "";
      Series s = null;

      if  (deployment != null)
      {
        legend = deployment.StationName;
        if  (!String.IsNullOrEmpty (deployment.DeploymentNum))
          legend += "-" + deployment.DeploymentNum;
        legend += " (" + startTimeAct.ToString("MMM/dd HH:mm") + ")";
        s = new Series (legend);
        s.BorderWidth = 2;
      }
      else
      {
        legend = "Cruise";
        s = new Series (legend);
        s.BorderWidth = 1;
        s.MarkerSize = 1;
      }

      s.ChartType = SeriesChartType.Point;
      s.ChartArea = "ChartArea1";
      s.Name = legend;
      s.XAxisType = AxisType.Primary;
      s.MarkerStyle = MarkerStyle.Circle;

      s.Points.Clear ();

      if  (dataSeries.latitudeMin  < minY)  minY = dataSeries.latitudeMin;
      if  (dataSeries.latitudeMax  > maxY)  maxY = dataSeries.latitudeMax;
      if  (dataSeries.longitudeMin < minX)  minX = dataSeries.longitudeMin;
      if  (dataSeries.longitudeMax > maxX)  maxX = dataSeries.longitudeMax;

      foreach  (PicesGPSDataPoint  dp in gpsData)
      {
        DataPoint dataPoint = new DataPoint (dp.Longitude, dp.Latitude);
        s.Points.Add (dataPoint);
      }

      if  (s.Points.Count > 0)
        s.Points[0].Label = startTimeStr;

      if  (s.Points.Count > 10)
        s.Points[s.Points.Count - 1].Label = endTimeStr;

      dataSeries.chartSeriesIndex = ProfileChart.Series.Count;
      ProfileChart.Series.Add (s);
    }  /* AddSeriesToChart */




    private  void  AddStationsToChart (PicesSipperStationList  stationsToPlot,
                                       ref double              minX,
                                       ref double              maxX,
                                       ref double              minY,
                                       ref double              maxY
                                      )
    {
      Series s = new Series ("Stations");
      s.MarkerSize = 20;
      s.MarkerStyle = MarkerStyle.Star5;
      s.ChartType = SeriesChartType.Point;
      s.ChartArea = "ChartArea1";
      s.Name = "";
      s.XAxisType = AxisType.Primary;
      s.Legend = "";
      s.Points.Clear ();

      foreach  (PicesSipperStation station in stationsToPlot)
      {
        double  lat = station.Latitude;
        double  lon = station.Longitude;

        if  ((lat == 0.0)  &&  (lon == 0.0))
          continue;

        if  (lat < minY)  minY = lat;
        if  (lat > maxY)  maxY = lat;
        if  (lon < minX)  minX = lon;
        if  (lon > maxX)  maxX = lon;

        DataPoint dataPoint = new DataPoint (lon, lat);
        dataPoint.Label = station.StationName;
        dataPoint.Font = new Font (FontFamily.GenericSerif, 12.0f);
        s.Points.Add (dataPoint);
      }

      ProfileChart.Series.Add (s);
    }  /* AddStationsToChart */



    private  void  UpdateChartAreas ()
    {
      goalie.StartBlock ();

      double  minX = double.MaxValue;
      double  maxX = double.MinValue;
      double  minY = double.MaxValue;
      double  maxY = double.MinValue;

      DateTime  minGpsDateTime = DateTime.MaxValue;
      DateTime  maxGpsDateTime = DateTime.MinValue;

      timeInterval = (int)TimeInterval.Value;
      //String  plotAction = PlotAction.SelectedText;

      PicesSipperDeploymentList  selectedDeployments = CurrentlySelectedDeployments (); 

      String titleLine = "Cruise: " + cruiseName;
      
      titleLine += "  Time-Interval: " + timeInterval;
     
      ProfileChart.Titles.Clear ();
      ProfileChart.Titles.Add (titleLine);

      if  (series.Count < 1)
      {
        ProfileChart.Series.Clear ();
        goalie.EndBlock ();
        return;
      }

      ChartArea ca = ProfileChart.ChartAreas[0];
      //ca.AxisY.Title = "Latitude";

      //ca.AxisX.Title = "Longitude";
      Font f = new Font (ca.AxisX.TitleFont.FontFamily, 10);
      ca.AxisX.TitleFont = f;
      f = new Font (ca.AxisY.TitleFont.FontFamily, 10);
      ca.AxisY.TitleFont = f;

      ProfileChart.Series.Clear ();

      // First we will plot cruise level series;  that is series that are not ties to a specific deployment.
      foreach  (DataSeriesToPlot dstp in series)
      {
        if  (dstp.deployment == null)
          AddSeriesToChart (dstp, ref minX, ref maxX, ref minY, ref maxY, ref minGpsDateTime, ref maxGpsDateTime);
      }

      Dictionary<String, PicesSipperStation>  stationsPlotted = new Dictionary<string,PicesSipperStation> ();
      PicesSipperDeploymentList  deploymentsPlotted = new PicesSipperDeploymentList ();
      // Second we plot series that is specific to a deployment.
      foreach  (DataSeriesToPlot dstp in series)
      {
        if  (dstp.deployment != null)
        {
          String  stationName = dstp.deployment.StationName;
          AddSeriesToChart (dstp, ref minX, ref maxX, ref minY, ref maxY, ref minGpsDateTime, ref maxGpsDateTime);
          deploymentsPlotted.Add (dstp.deployment);
        }
      }

      String[]  stationNamesToPlot = deploymentsPlotted.ExtractStationNames ();
      PicesSipperStationList  stationsToPlot = new PicesSipperStationList ();
      foreach (String stationName in stationNamesToPlot)
      {
        PicesSipperStation  stationToPlot = stations.LookUpByStationName (stationName);
        if  (stationToPlot != null)
        {
          stationsToPlot.Add (stationToPlot);
          stationsPlotted.Add (stationToPlot.StationName, stationToPlot);
        }
      }

      // Third we add stations that were not in one of the specified deployments but awe in the area of the deployments.
      // Find other stations in plot area.
      // Degrees/Kilo-Meter = 1/111  There are 111 Km/Degree of latitude

      double  paddingKms = (double)PaddingKms.Value;
      double  latPadding  =  paddingKms * (1.0 / 111.0);  // Will add 5km's of padding to latitude.
      double  degreesToRads = Math.PI / 180.0;
      double  kmsPerDegLong = 111.6 * Math.Cos ((minY + maxY) / 2.0  * degreesToRads);
      double  longPadding = paddingKms * (1.0 / (kmsPerDegLong));

      PicesSipperStationList  stationsInRange = 
         mainWinConn.SipperStationsLoadByGpsRange (minY - latPadding, maxY + latPadding, minX - longPadding, maxX + longPadding);

      if  (stationsInRange != null)
      {
        foreach  (PicesSipperStation  stationToPlot in stationsInRange)
        {
          if  (!stationsPlotted.ContainsKey (stationToPlot.StationName))
          {
            stationsPlotted.Add (stationToPlot.StationName, stationToPlot);
            stationsToPlot.Add (stationToPlot);
          }
        }
      }
      
      AddStationsToChart (stationsToPlot, ref minX, ref maxX, ref minY, ref maxY);

      double  latitudeMid = (maxY + minY) / 2.0;

      float  plotAreaHeight = ProfileChart.Height;
      float  plotAreaWidth  = ProfileChart.Width;

      double  xRange = maxX - minX;
      double  yRange = maxY - minY;
      double  xRangeAdj = xRange * Math.Cos(latitudeMid * degToRad);

      double  yDensity = yRange    / plotAreaHeight;
      double  xDenisty = xRangeAdj / plotAreaWidth;

      if  (xDenisty > yDensity)
      {
        yRange = xDenisty * plotAreaHeight;
        double  leftOver = yRange - (maxY - minY);
        minY = minY - leftOver / 2.0;
        maxY = maxY + leftOver / 2.0;
      }
      else
      {
        xRangeAdj = yDensity * plotAreaWidth;
        xRange = xRangeAdj / Math.Cos(latitudeMid * degToRad);
        double  leftOver = xRange - (maxX - minX);
        minX = minX - leftOver / 2.0;
        maxX = maxX + leftOver / 2.0;
      }

      ca.AxisX.Minimum = minX;
      ca.AxisX.Maximum = maxX;
      ca.AxisY.Minimum = minY;
      ca.AxisY.Maximum = maxY;

      ProfileChart.ChartAreas[0].RecalculateAxesScale ();

      goalie.EndBlock ();
    }  /* UpdateChartAreas */


    
    private  void  SchedulePlotRequests ()
    {
      //String  plotAction = PlotAction.SelectedText;

      PicesSipperDeploymentList  deploymentsRequested = CurrentlySelectedDeployments ();
      if  (deploymentsRequested == null)  
        deploymentsRequested = new PicesSipperDeploymentList ();

      plotRequests = new List<PlotRequest> ();

      foreach  (PicesSipperDeployment  deployment in  deploymentsRequested)
      {
        plotRequests.Add (new PlotRequest (deployment, Color.Black));
      }

      plotCruise = PlotCruiseField.Checked;
    }  /* SchedulePlotRequests */



    private void PlotButton_Click (object sender, EventArgs e)
    {
      List<String>  errors = ValidateFields ();
      if  (errors != null)
      {
        String  errMsg = "Errors is class selection";
        foreach (String s in errors)
          errMsg += "\n" + s;
        MessageBox.Show (this, errMsg, "Validating Class Selection", MessageBoxButtons.OK);
      }
      else
      {
        DisableControls ();
        SaveConfiguration ();
        PlotButton.Enabled = false;
        if  (buildPlotDataRunning)
          return;

        SchedulePlotRequests ();

        timer1.Enabled = true;
        Thread t = new Thread (new ThreadStart (BuildPlotData));
        t.Start ();
      }
    }



    private void RePlotButton_Click (object sender, EventArgs e)
    {
      UpdateChartAreas ();
    }



    int ticks = 0;
    private void timer1_Tick (object sender, EventArgs e)
    {
      ++ticks;

      String msg = statusMsgs.GetNextMsg ();
      while  (msg != null)
      {
        Status.Text = msg;
        msg = statusMsgs.GetNextMsg ();
      }

      if   (buildPlotDataRunning)
      {
        if  ((ticks % 20) == 0)
          UpdateChartAreas ();
      }

      if  (buildPlotDataDone)
      {
        UpdateChartAreas ();
        PlotButton.Enabled = true;
        buildPlotDataDone = false;
        cancelRequested = false;
        EnableControls ();
      }
    }



    private void ChartGPS_Resize (object sender, EventArgs e)
    {
      int  heightDelta = Height - heightLast;
      int  widthDelta  = Width  - widthLast;

      ProfileChart.Height += heightDelta;
      ProfileChart.Width  += widthDelta;

      heightLast = Height;
      widthLast  = Width;

      UpdateChartAreas ();
    }



    private void ChartGPS_SizeChanged (object sender, EventArgs e)
    {
      if  (WindowState == FormWindowState.Maximized)
      {
        // Looks like user has pressed the Maximized button.  We have to trap it here because
        // the ResizeEnd event does not trap when form is Maximized.
        //PicesCommanderFormResized ();
        ChartGPS_Resize (sender, e);
        formIsMaximized = true;
      }
      else if  (WindowState == FormWindowState.Normal)
      {
        if  (formIsMaximized)
        {
          // We normally trap the ResizeEnd event;  but when the form was already maximized and the user
          // presses the button to un-maximize.  the ResizeEnd does not trap that.  So we check to 
          // see if the form was already maximize.  If so then we resized the form.
          //PicesCommanderFormResized ();
          ChartGPS_Resize (sender, e);
          formIsMaximized = false;
        }
      }
    }



    private  void  CopyChartToClipboard (Object sender, EventArgs e)
    {
      using (System.IO.MemoryStream ms = new System.IO.MemoryStream ())
      {
        ProfileChart.SaveImage (ms, ChartImageFormat.Bmp);
        Bitmap bm = new Bitmap(ms);
        Clipboard.SetImage(bm);
      }
    }


    private  void  SaveChartToDisk (Object sender, EventArgs e)
    {
      SaveFileDialog sfd = new SaveFileDialog ();
      sfd.Filter = "JPeg Image|*.jpg|Bitmap Image|*.bmp|Gif Image|*.gif|Tab-Del|*.txt";
      sfd.Title = "Save chart as Image File";
      sfd.DefaultExt = "jpg";
      sfd.AddExtension = true;
      sfd.OverwritePrompt = true;
      //sfd.CheckFileExists = true;
      DialogResult dr = sfd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        String  fn = sfd.FileName;
        System.IO.FileInfo fi = new System.IO.FileInfo (fn);
        String ext = fi.Extension.ToLower ();

        ChartImageFormat cif = ChartImageFormat.Jpeg;
        if  (ext == ".jpg")
          cif = ChartImageFormat.Jpeg;

        else if  (ext == ".bmp")
          cif = ChartImageFormat.Bmp;

        else if  (ext == ".gif")
          cif = ChartImageFormat.Gif;
        
        else if  (ext == ".txt")
        {
          SaveTabDelToDisk (fn);
          return;
        }
        
        ProfileChart.SaveImage (fn, cif);
      }
    }


    
    private  void  SaveTabDelToDisk (Object sender, EventArgs e)
    {
      SaveFileDialog sfd = new SaveFileDialog ();
      sfd.Filter = "Tab delimited data to disk|*.txt";
      sfd.Title = "Save GPS info as tab-delimited data to disk.";
      sfd.DefaultExt = "txt";
      sfd.AddExtension = true;
      sfd.OverwritePrompt = true;
      //sfd.CheckFileExists = true;
      DialogResult dr = sfd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        String  fn = sfd.FileName;
        SaveTabDelToDisk (fn);
      }
    }



    private  void  SaveTabDelToDisk (String fileName)
    {
      System.IO.StreamWriter sw = null;
      try  {sw = new System.IO.StreamWriter (fileName, false, System.Text.Encoding.ASCII);}
      catch (Exception) {sw = null;}

      if  (sw == null)
      {
        MessageBox.Show (this, "Could not open file[" + fileName + "]", "Save Tab-Delimited", MessageBoxButtons.OK);
        return;
      }
      WriteTabDelToStream (sw);
      sw.Close ();
      sw = null;

      MessageBox.Show (this, "GPS Data written to \"" + fileName + "\".", "", MessageBoxButtons.OK);
    }


    private  void  SaveTabDelToClipBoard (Object sender, EventArgs e)
    {
      StringWriter  sw = new StringWriter ();
      WriteTabDelToStream (sw);
      Clipboard.SetText (sw.ToString ());
      sw.Close ();
      sw = null;
    }


    
    private  void  WriteTabDelToStream (TextWriter tw)
    {
      DateTime  curTime = DateTime.Now;
      tw.WriteLine ("ChartGPS"); 
      tw.WriteLine ("DateTime" + "\t" + curTime.ToString ("yyyy/MM/dd HH:mm:ss")); 
      tw.WriteLine ();
      foreach (DataSeriesToPlot dstp in series)
      {
        PicesGPSDataPointList  gpsData    = dstp.data;
        PicesSipperDeployment  deployment = dstp.deployment;

        tw.WriteLine ("Deployment"            + "\t" + deployment.CruiseName + "\t" + deployment.StationName + "\t" + deployment.DeploymentNum);
        tw.WriteLine ("DeploymentDescription" + "\t" + deployment.Description);
        tw.WriteLine ("SyncTimeStampActual"   + "\t" + deployment.SyncTimeStampActual.ToString ("yyyy/MM/dd HH:mm:ss"));
        tw.WriteLine ("SyncTimeStampCTD"      + "\t" + deployment.SyncTimeStampCTD.ToString ("yyyy/MM/dd HH:mm:ss"));
        tw.WriteLine ("SyncTimeStampGPS"      + "\t" + deployment.SyncTimeStampGPS.ToString ("yyyy/MM/dd HH:mm:ss"));

        tw.WriteLine("GpsUtcTime"  + "\t" + "Latitude" + "\t" + "Longitude" + "\t" + "Latitude(hh:mm)" + "\t" + "Longitude(hh:mm)" +  "\t" + "COG" + "\t" + "SOG(kts)");
        foreach  (PicesGPSDataPoint dp in gpsData)
        {
          tw.WriteLine (dp.GpsUtcTime.ToString ("yyyy/MM/dd HH:mm:ss")    + "\t" + 
                        dp.Latitude.ToString ("##0.00000000")             + "\t" + 
                        dp.Longitude.ToString ("##0.00000000")            + "\t" + 
                        PicesMethods.LatitudeToString  (dp.Latitude,  4)  + "\t" + 
                        PicesMethods.LongitudeToString (dp.Longitude, 4)  + "\t" + 
                        dp.CourseOverGround.ToString ("##0.0")            + "\t" +
                        dp.SpeedOverGround.ToString ("##0.0")
                        );
        }
        tw.WriteLine ("----EndOfDeoploymemt----");
        tw.WriteLine ();
      }

      tw.WriteLine ("End of ChartGPS");
    }



    private void ChartGPS_FormClosing (object sender, FormClosingEventArgs e)
    {
      if  (buildPlotDataRunning)
      {
        if (e.CloseReason == CloseReason.WindowsShutDown)
        {
          CancelBackGroundThread ();
          while  (buildPlotDataRunning)
          {
            Thread.Sleep (100);
          }
        }
        else
        {
          MessageBox.Show (this, "Plot data is being collected; press \"Cancel Button\" to Stop before exiting.", "Form Closing", MessageBoxButtons.OK);
          e.Cancel = true;
          return;
        }
      }

      SaveConfiguration ();
      
      mainWinConn.Close ();
      mainWinConn = null;
      GC.Collect ();
    }



    private void CancelPlotButton_Click (object sender, EventArgs e)
    {
      if  (buildPlotDataRunning)
        CancelBackGroundThread ();
    }


    private void DeploymentsToPlot_Format (object sender, ListControlConvertEventArgs e)
    {
      if  (e.ListItem == null)
        return;
      PicesSipperDeployment  d = (PicesSipperDeployment)e.ListItem;
      String  m = d.StationName;
      if (!String.IsNullOrEmpty (d.DeploymentNum))
        m += "-" + d.DeploymentNum;

      m += " (" + d.DateTimeStart.ToString ("yyyy-MMM-dd HH:mm") + ")";

      e.Value = m;
    }




    PicesGPSDataPoint  lastClosestPoint       = null;
    int                lastClosestSeriesIndex = -1;
    int                lastClosestPointIndex  = -1;

    private  void  HighLightClosestPoint (int                    closestPointSeriesIndex,
                                          int                    closestPointPointIndex,
                                          PicesGPSDataPoint      closestPoint,
                                          PicesSipperDeployment  closestDeployment
                                         )
    {
     
      TimeSpan  adjGpsToAct = cruiseAdjGpsToActTime;
      if  (closestDeployment != null)
        adjGpsToAct = closestDeployment.SyncTimeStampActual - closestDeployment.SyncTimeStampGPS;
      
      DateTime adjDateTime = closestPoint.GpsUtcTime.Add (adjGpsToAct);

      if  ((lastClosestSeriesIndex >= 0)  &&  (lastClosestSeriesIndex < ProfileChart.Series.Count))
      {
        if  ((lastClosestPointIndex >= 0)  &&  (lastClosestPointIndex < ProfileChart.Series[lastClosestSeriesIndex].Points.Count))
        {
          Series  s = ProfileChart.Series[lastClosestSeriesIndex];
          DataPoint dp = s.Points[lastClosestPointIndex];
          dp.Label = "";
          dp.MarkerSize  = s.MarkerSize;
          dp.MarkerStyle = s.MarkerStyle;
        }
      }

      lastClosestPoint       = null;
      lastClosestSeriesIndex = -1;
      lastClosestPointIndex  = -1;

      if  ((closestPointSeriesIndex >= 0)  &&  (closestPointSeriesIndex < ProfileChart.Series.Count))
      {
        if  ((closestPointPointIndex >= 0)  &&  (closestPointPointIndex < ProfileChart.Series[closestPointSeriesIndex].Points.Count))
        {
          Series  s = ProfileChart.Series[closestPointSeriesIndex];
          DataPoint dp = s.Points[closestPointPointIndex];

          dp.Label = adjDateTime.ToString ("HH:mm");
          dp.MarkerStyle = MarkerStyle.Diamond;
          dp.MarkerSize = s.MarkerSize * 3;
          lastClosestPoint       = closestPoint;
          lastClosestSeriesIndex = closestPointSeriesIndex;
          lastClosestPointIndex  = closestPointPointIndex;
        }
      }
    }  /* HighLightClosestPoint */



    private void ProfileChart_MouseClick (object sender, MouseEventArgs e)
    {
      Point  p = e.Location;
      //MouseClickLocation.Text = p.X.ToString () + "," + p.Y.ToString ();
      ChartArea  ca = ProfileChart.ChartAreas[0];
      double longitude = ca.AxisX.PixelPositionToValue ((double)p.X);
      double latitude  = ca.AxisY.PixelPositionToValue ((double)p.Y);

      // Determine Threshold.
      int  thInPixels = 25;
      double  x = ca.AxisX.PixelPositionToValue ((double)(p.X + thInPixels));
      double  degLongPerPixel = Math.Abs (x - longitude) / thInPixels;

      double  y = ca.AxisY.PixelPositionToValue ((double)(p.Y + thInPixels));
      double  degLatPerPixel = Math.Abs (y - latitude) / thInPixels;

      double  longTH = degLongPerPixel * thInPixels;
      double  latTH  = degLatPerPixel  * thInPixels;

      PicesGPSDataPoint  closestPoint = null;
      int                closestPointIndex = -1;
      int                closestPointSeriesIndex = -1;
      double             closestPointDistSquare = double.MaxValue;
      PicesSipperDeployment  closestDeployment = null;
      foreach  (DataSeriesToPlot  dstp in series)
      {
        if  (dstp.WithInThreshold (longitude, longTH, latitude, latTH))
        {
          double distSquare = double.MaxValue;
          int    closestPointInSeriesIndex = -1;
          PicesGPSDataPoint  closestPointInSeries = dstp.ClosestPoint (longitude, longTH, latitude, latTH, ref closestPointInSeriesIndex, ref distSquare);
          if  (closestPointInSeries != null)
          {
            if  (distSquare < closestPointDistSquare)
            {
              closestPointIndex       = closestPointInSeriesIndex;
              closestPointSeriesIndex = dstp.chartSeriesIndex;
              closestPointDistSquare  = distSquare;
              closestPoint            = closestPointInSeries;
              closestDeployment       = dstp.deployment;
            }
          }
        }
      }

      if  (closestPoint != null)
      { 
        HighLightClosestPoint (closestPointSeriesIndex, closestPointIndex, closestPoint, closestDeployment);
        String  gpsStr = PicesMethods.LatitudeLongitudeToString (closestPoint.Latitude, closestPoint.Longitude);
        CurGPSLocation.Text = gpsStr;
        COGField.Text = closestPoint.CourseOverGround.ToString ("##0.0") + " deg's";
        SOGField.Text = closestPoint.SpeedOverGround.ToString  ("#0.0") + " kts";

        if  (closestDeployment != null)
          DeploymentHighlighted.Text = closestDeployment.ShortDescription;
        else
          DeploymentHighlighted.Text = "Cruise";
      }
    }



    private void ClearAllButton_Click (object sender, EventArgs e)
    {
      for  (int x = 0;  x < DeploymentsToPlot.Items.Count;  ++x)
        DeploymentsToPlot.SetItemCheckState (x, CheckState.Unchecked);
    }



    private void SelectAllButton_Click (object sender, EventArgs e)
    {
      for  (int x = 0;  x < DeploymentsToPlot.Items.Count;  ++x)
        DeploymentsToPlot.SetItemCheckState (x, CheckState.Checked);
    }
  }
}
