using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Threading;
using System.IO;

using SipperFile;
using PicesInterface;

namespace PicesCommander
{
  public partial class ChartSizeDistribution : Form
  {
    PicesClassList  classes       = null;
    PicesClassList  activeClasses = null;
    String          rootDir       = "";

    private  String  cruise       = null;
    private  String  station      = null;
    private  String  deployment   = null;

    private  String  infinityStr = ((char)8734).ToString ();

    private  PicesDataBase    mainWinConn = null;
    private  PicesDataBase    threadConn  = null;  /**< This connection will only be use by the background thread. */

    private  PicesMsgQueue    statusMsgs = null;
    private  PicesMsgQueue    msgQueue   = null;
    private  PicesRunLog      runLog     = null;

    private  String           criteriaStr = "";

    private  PicesClass       classToPlot = null;
    private  bool             includeSubClasses = false;

    private  PicesImageSizeDistribution     downCast = null;
    private  PicesImageSizeDistribution     upCast   = null;
    private  PicesImageSizeDistribution     bucketsDisplayed = null;  // Will contain the data that is actually displayed in the Chart; depending of Cast options.
    private  float[]                        integratedDensityDistribution = null;

    private  String  cast              = "Down";
    private  char    statistic         = '0';
    private  float   growthRate        = 1.1f;
    private  float   initialSizeValue = 0.01f;

    private  PicesGoalKeeper  goalie = new PicesGoalKeeper ("ChartSizeDistribution");

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

    private  int     selectedSizeBucket = -1;

    private  String  lastSaveDirectory = "";

    private  Font titleFont     = new Font (FontFamily.GenericSansSerif, 12);
    private  Font axisLabelFont = new Font (FontFamily.GenericSansSerif, 12);
    private  Font dataPoingFont = new Font (FontFamily.GenericSansSerif, 12, FontStyle.Bold);


    public ChartSizeDistribution (String          _cruise,
                                  String          _station,
                                  String          _deployment,
                                  PicesClass      _classToPlot,
                                  PicesClassList  _classes,
                                  PicesClassList  _activeClasses,
                                  String          _rootDir
                                 )
    {
      cruise        = _cruise;
      station       = _station;
      deployment    = _deployment;
      classToPlot   = _classToPlot;
      classes       = _classes;
      activeClasses = _activeClasses;
      rootDir       = _rootDir;

      lastSaveDirectory = OSservices.AddSlash (PicesSipperVariables.PicesReportDir ()) + "AbundanceReports";

      statusMsgs = new PicesMsgQueue ("ChartSizeDistribution-StatusMsgs");
      msgQueue   = new PicesMsgQueue ("ChartSizeDistribution-RunLog");
      runLog     = new PicesRunLog (msgQueue);

      mainWinConn = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);

      configFileName = OSservices.AddSlash (PicesSipperVariables.PicesConfigurationDirectory ()) + "ChartSizeDistribution.cfg";

      InitializeComponent ();
    }

   

    private  void  AddToStr (ref String existingStr, String  addOn)
    {
      if  (String.IsNullOrEmpty (addOn))
        return;

      if  (!String.IsNullOrEmpty (existingStr))
        existingStr += ", ";
      existingStr += addOn;
    }



    private void ChartAbundanceByDeployment_Load (object sender, EventArgs e)
    {
      heightLast = Height;
      widthLast  = Width;

      Cruise_.Text     = cruise;
      Station_.Text    = station;
      Deployment_.Text = deployment;

      if  (classToPlot != null)
        ClassToPlot.Text = classToPlot.Name;

      CastField.SelectedItem = CastField.Items[0];
      CastField.Text         = (String)CastField.Items[0];

      SizeStatisticField.SelectedItem = SizeStatisticField.Items[0];
      SizeStatisticField.Text         = (String)SizeStatisticField.Items[0];

      LoadConfigurationFile ();
    }


    private  void  DisableControls ()
    {
      PlotButton.Enabled         = false;
      IncludeSubClasses.Enabled  = false;
      InitialSizeField.Enabled   = false;
      MaxSizeField.Enabled       = false;
      GrowthRateField.Enabled    = false;
      SizeStatisticField.Enabled = false;
    }


    private  void  EnableControls ()
    {
      PlotButton.Enabled         = true;
      IncludeSubClasses.Enabled  = true;
      InitialSizeField.Enabled   = true;
      MaxSizeField.Enabled       = true;
      GrowthRateField.Enabled    = true;
      SizeStatisticField.Enabled = true;
    }


    private  void  SaveConfiguration ()
    {
      System.IO.StreamWriter  o = null;
      try{o = new System.IO.StreamWriter (configFileName);}  catch  (Exception){o = null; return;}
      
      o.WriteLine ("//ChartSizeDistribution Configuration File");
      o.WriteLine ("//");
      o.WriteLine ("//DateTime Saved" + "\t" + DateTime.Now.ToString ("F"));
      o.WriteLine ("//");
      o.WriteLine ("WidthLast"          + "\t" + widthLast);
      o.WriteLine ("HeightLast"         + "\t" + heightLast);
      o.WriteLine ("Maximized"          + "\t" + (formIsMaximized ? "YES":"NO"));
      o.WriteLine ("ClassToPlot"        + "\t" + ClassToPlot.Text);
      o.WriteLine ("IncludeSubClasses"  + "\t" + IncludeSubClasses.Checked);
      o.WriteLine ("SizeStatistic"      + "\t" + SizeStatisticField.Text);
      o.WriteLine ("InitialSize"        + "\t" + InitialSizeField.Value.ToString ("###0.000"));
      o.WriteLine ("MaxSize"            + "\t" + MaxSizeField.Value);
      o.WriteLine ("GrowthRate"         + "\t" + GrowthRateField.Value);
      if  (SizeStatisticField.SelectedItem != null)
        o.WriteLine ("SizeStatistic"    + "\t" + SizeStatisticField.SelectedItem.ToString ());
      o.WriteLine ("LastSaveDirectory"  + "\t" + lastSaveDirectory);
      o.Close ();
      o = null;
    }


    private  void  SetSizeStatistic (String  s)
    {
      foreach  (Object o in SizeStatisticField.Items)
      {
        if  (o.ToString ().Equals (s, StringComparison.InvariantCultureIgnoreCase))
        {
          SizeStatisticField.SelectedItem = o;
          return;
        }
      }
      SizeStatisticField.SelectedIndex = 0;
    } /* SetSizeStatistic */



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

          case  "ClassToPlot":
            ClassToPlot.Text = fieldValue;
            break;

          case  "IncludeSubClasses":
            IncludeSubClasses.Checked = PicesKKStr.StrToBool (fieldValue);
            break;
            
          case  "SizeStatistic":
            SetSizeStatistic (fieldValue);
            break;

          case  "InitialSize":
            float  initialSize = PicesKKStr.StrToFloat (fieldValue);
            if  ((initialSize <= 0.0)  ||  (initialSize >= 100.0f))
              initialSize = 0.10f;
            InitialSizeField.Value = (decimal)initialSize;
            break;

          case  "MaxSize":
            float  maxSize = PicesKKStr.StrToFloat (fieldValue);
            if  ((maxSize <= 0.0)  ||  (maxSize < (float)InitialSizeField.Value))
              maxSize = Math.Max (1000.0f, (float)InitialSizeField.Value);

            MaxSizeField.Value = (decimal)maxSize;
            break;
          
          case  "GrowthRate":
            float  growthRate = PicesKKStr.StrToFloat (fieldValue);
            if  (growthRate <= 1.0f)
              growthRate = 1.1f;
            GrowthRateField.Value = (decimal)growthRate;
            break;

          case  "LastSaveDirectory":
            char[]  whiteSpaceChars = {'\n', '\r', '\t', ' '};
            lastSaveDirectory = fieldValue.Trim (whiteSpaceChars);
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

      if  (SizeStatisticField.SelectedItem == null)
        SizeStatisticField.SelectedItem = SizeStatisticField.Items[0];

      OnResizeEnd (new EventArgs ());
    }  /* LoadConfigurationFile */



    private  List<String>  ValidateFields ()
    {
      List<String>  errors = new List<string> ();
      switch  (SizeStatisticField.Text)
      {
      case "Pixel Area":   statistic = '0';   break;
      case "Diameter":     statistic = '1';   break;
      case "Volume (SBv)": statistic = '2';   break;
      case "Volume (EBv)": statistic = '3';   break;
      }

      cast = CastField.Text;

      growthRate       = (float)GrowthRateField.Value;
      initialSizeValue = (float)InitialSizeField.Value;

      includeSubClasses = IncludeSubClasses.Checked;

      if  (String.IsNullOrEmpty (ClassToPlot.Text))
        errors.Add ("You must specify class to plot.");
      else
        classToPlot = PicesClassList.GetUniqueClass (ClassToPlot.Text, "");

      if  (errors.Count < 1)
        return null;
      else
        return errors;
    }

    

    private  void  SizeDistributionForAClass (PicesClass                      c,
                                              bool                            includeChildren,
                                              ref PicesImageSizeDistribution  downCastAcumulated,
                                              ref PicesImageSizeDistribution  upCastAcumulated
                                             )
    {
      statusMsgs.AddMsg ("Extracting for Class[" + c.Name + "]");
      sbyte  ch = (sbyte)statistic;
      PicesImageSizeDistribution  classDownCast = null;
      PicesImageSizeDistribution  classUpCast = null;
      threadConn.ImagesSizeDistributionByDepth (this.cruise, this.station, this.deployment, c.Name,
                                                5.0f,
                                                ch,
                                                (float)InitialSizeField.Value,
                                                (float)GrowthRateField.Value,
                                                (float)MaxSizeField.Value,
                                                ref classDownCast,
                                                ref classUpCast
                                               );

      if  (classDownCast != null)
      {
        if  (downCastAcumulated == null)
          downCastAcumulated = classDownCast;
        else
          downCastAcumulated.AddIn (classDownCast, runLog);
      }

      if  (classUpCast != null)
      {
        if  (upCastAcumulated == null)
          upCastAcumulated = classUpCast;
        else
          upCastAcumulated.AddIn (classUpCast, runLog);
      }

      if  (includeChildren)
      {
        foreach  (PicesClass  pc in  c.Children)
        {
          if  (cancelRequested)
            break;
          SizeDistributionForAClass (pc, includeChildren, ref downCastAcumulated, ref upCastAcumulated);
        }
      }
    }  /* SizeDistributionForAClass */
    
  



    
    /// <summary>
    /// This method will be ran as a separate thread; it is responsible for collecting all the data needed to generate the plot.
    /// </summary>
    private  void  BuildPlotData ()
    {
      if  (buildPlotDataRunning)
        return;

      PicesDataBase.ThreadInit ();

      threadConn = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);

      buildPlotDataRunning = true;

      classToPlot = PicesClassList.GetUniqueClass (ClassToPlot.Text, "");

      sbyte  ch = (sbyte)statistic;
      downCast = null;
      upCast = null;

      goalie.StartBlock ();
      
      SizeDistributionForAClass (classToPlot, 
                                 IncludeSubClasses.Checked, 
                                 ref downCast, 
                                 ref upCast
                                );

      if  (cast == "Down")
        bucketsDisplayed = downCast;

      else if  (cast == "Up")
        bucketsDisplayed = upCast;

      else
      {
        bucketsDisplayed = new PicesImageSizeDistribution (downCast);
        bucketsDisplayed.AddIn (upCast, runLog);
      }

      integratedDensityDistribution = bucketsDisplayed.IntegratedDensityDistribution ();

      threadConn.Close ();
      threadConn = null;
      GC.Collect ();
      PicesDataBase.ThreadEnd ();

      goalie.EndBlock ();
      
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



    private  String  SubInSuperScriptExponent (String s)
    {
      int x = s.IndexOf("-3");
      while  (x >= 0)
      {
        s = s.Substring (0, x) + "\u00B3" + s.Substring (x + 2);
        x = s.IndexOf("-3");
      }

      x = s.IndexOf("-2");
      while  (x >= 0)
      {
        s = s.Substring (0, x) + "\u00B2" + s.Substring (x + 2);
        x = s.IndexOf("-2");
      }

      return s;
    }  /* SuperScriptExponent */




    private  void  UpdateChartAreas ()
    {
      if  (integratedDensityDistribution == null)
        return;

      goalie.StartBlock ();

      String  t1 = "Abundance by Size";
      switch  (statistic)
      {
      case '0': t1 = "Abundance by Size";                       break;
      case '1': t1 = "Abundance by Estimated Diameter";         break;
      case '2': t1 = "Abundance by Estimated Spheroid Volume";  break;
      case '3': t1 = "Abundance by Estimated Ellipsoid Volume"; break;
      }

      String  t2 = "Cruise: " + cruise + "  Station: " + station;
      if  (String.IsNullOrEmpty (deployment))
        t2 += "  Deployment: " + deployment;

      t2 += "  Class: " + classToPlot.Name;

      ProfileChart.Titles.Clear ();
      ProfileChart.Titles.Add (new Title (t1, Docking.Top, titleFont, Color.Black));
      ProfileChart.Titles.Add (new Title (t2, Docking.Top, titleFont, Color.Black));

      if  (!String.IsNullOrEmpty (criteriaStr))
        ProfileChart.Titles.Add (new Title (criteriaStr, Docking.Top, titleFont, Color.Black));

      ProfileChart.Series.Clear ();

      ChartArea ca = ProfileChart.ChartAreas[0];

      Series s = new Series ("Size Distribution");
      s.ChartArea = "ChartArea1";
      s.ChartType = SeriesChartType.Column;

      float[]  startValues  = bucketsDisplayed.SizeStartValues ();
      float[]  endValues    = bucketsDisplayed.SizeEndValues   ();

      float  minX = float.MaxValue;
      float  minY = float.MaxValue;
      float  maxX = float.MinValue;
      float  maxY = float.MinValue;

      List<CustomLabel>  customLabels = new List<CustomLabel> ();
      for  (int x = 0;  x < integratedDensityDistribution.Length;  ++x)
      {
        float  sv = startValues[x];
        float  ev = sv * growthRate;
        if  (sv <= 0.0f)
        {
          sv = initialSizeValue / growthRate;
          ev = initialSizeValue;
        }
        float  midPoint = (sv + ev) / 2.0f;
        
        {
          double  svLog10 = Math.Log10 (sv);
          double  evLog10 = Math.Log10 (ev);
          CustomLabel cl = null;
          if  (midPoint < 1.0f)
            cl = new CustomLabel (svLog10, evLog10, midPoint.ToString ("#0.000"), 0, LabelMarkStyle.SideMark);
          else if  (midPoint < 10.0f)
            cl = new CustomLabel (svLog10, evLog10, midPoint.ToString ("#0.00"), 0, LabelMarkStyle.SideMark);
          else if  (midPoint < 100.0f)
            cl = new CustomLabel (svLog10, evLog10, midPoint.ToString ("##0.0"), 0, LabelMarkStyle.SideMark);
          else 
            cl = new CustomLabel (svLog10, evLog10, midPoint.ToString ("##,##0"), 0, LabelMarkStyle.SideMark);
          customLabels.Add (cl);
        }

        double  d = (double)integratedDensityDistribution[x];
        float log10D = (float)Math.Log10 (d + 1.0);

        minY = Math.Min (minY, log10D);
        maxY = Math.Max (maxY, log10D);
        minX = Math.Min (minX, midPoint);
        maxX = Math.Max (maxX, midPoint);
        DataPoint dp = new DataPoint (midPoint, log10D);
        s.Points.Add (dp);
      }
      s.XAxisType = AxisType.Primary;
      s.YAxisType = AxisType.Primary;
      ProfileChart.ChartAreas[0].AxisY.LogarithmBase = 10.0;
      ProfileChart.ChartAreas[0].AxisY.LabelStyle.Format = "##,###,##0";

      {
        ca.AxisX.CustomLabels.Clear ();
        ca.AxisX.CustomLabels.Add (customLabels[1]);
        int n = 8;
        float  interSpace = (float)customLabels.Count / (float)(n + 1);
        for  (int x = 1;  x <= n;  ++x)
        {
          int  idx = 1 + (int)((float)x * interSpace + 0.5f);
          ca.AxisX.CustomLabels.Add (customLabels[idx]);
        }

        ca.AxisX.CustomLabels.Add (customLabels[customLabels.Count - 1]);
      }


      ca.AxisY.Title = SubInSuperScriptExponent ("Log 10  Abundance");

      switch  (statistic)
      {
      case '0':
        ca.AxisX.Title = SubInSuperScriptExponent ("area(m-2)");
        //ca.AxisX.LabelStyle.Format =  "##,##0.000";
        break;

      case '1':
        ca.AxisX.Title = SubInSuperScriptExponent ("Length(mili-meters)");
        //ca.AxisX.LabelStyle.Format =  "##00.00";
        break;

      case '2':
        ca.AxisX.Title = SubInSuperScriptExponent ("Volume(mm-3)");
        //ca.AxisX.LabelStyle.Format =  "##0.0000";
        break;

      case '3':
        ca.AxisX.Title = SubInSuperScriptExponent ("Volume(mm-3)");
        //ca.AxisX.LabelStyle.Format =  "##0.0000";
        break;
      }

      ca.AxisX.IsLogarithmic    = true;
      //ca.AxisX.Minimum          = 0.0;
      ca.AxisX.Maximum          = maxX;
      ca.AxisX.LabelStyle.Angle = 45;
      ca.AxisX.LabelStyle.Font  = axisLabelFont;
      ca.AxisX.TitleFont        = axisLabelFont;

      ca.AxisY.Minimum          = 0.0;
      ca.AxisY.IsLogarithmic    = false;
      ca.AxisY.LabelStyle.Font  = axisLabelFont;
      ca.AxisY.TitleFont        = axisLabelFont;
      //ca.AxisX.IsLogarithmic = true;
 
      s.BorderWidth = 2;
      ProfileChart.Series.Add (s);

      try
      {
        ProfileChart.ChartAreas[0].RecalculateAxesScale ();
      }
      catch (Exception e)
      {
        runLog.Writeln (e.ToString ());
      }

      goalie.EndBlock ();
    }  /* UpdateChartAreas */



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

        timer1.Enabled = true;
        Thread t = new Thread (new ThreadStart (BuildPlotData));
        t.Start ();
      }
    }


    int ticks = 0;
    private void timer1_Tick (object sender, EventArgs e)
    {
      ++ticks;

      String msg = statusMsgs.GetNextMsg ();
      while  (msg != null)
      {
        Status.AppendText (msg + "\n");
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


    private void ChartAbundanceByDeployment_Resize (object sender, EventArgs e)
    {
      int  heightDelta = Height - heightLast;
      int  widthDelta  = Width  - widthLast;

      ProfileChart.Height += heightDelta;
      ProfileChart.Width  += widthDelta;

      Status.Height        += heightDelta;
      PlotButton.Top       += heightDelta;
      CancelPlotButton.Top += heightDelta;
   
      heightLast = Height;
      widthLast  = Width;
    }



    private void ChartAbundanceByDeployment_SizeChanged (object sender, EventArgs e)
    {
      if  (WindowState == FormWindowState.Maximized)
      {
        // Looks like user has pressed the Maximized button.  We have to trap it here because
        // the ResizeEnd event does not trap when form is Maximized.
        //PicesCommanderFormResized ();
        ChartAbundanceByDeployment_Resize (sender, e);
        formIsMaximized = true;
      }
      else if  (WindowState == FormWindowState.Normal)
      {
        if  (formIsMaximized)
        {
          // We normally trap the ResizeEnd event;  but when the form was already maximized and the user
          // presses the button to maximize.  the ResizeEnd does not trap that.  So we check to 
          // see if the form was already maximize.  If so then we resized the form.
          //PicesCommanderFormResized ();
          ChartAbundanceByDeployment_Resize (sender, e);
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
      sfd.Title = "Save Abundance info as tab-delimited data to disk.";
      sfd.DefaultExt = "txt";
      sfd.AddExtension = true;
      sfd.OverwritePrompt = true;

      String fn = "AbundanceByDeployment";
      if  (!String.IsNullOrWhiteSpace (cruise))
      {
        fn += ("_" + cruise);
        if  (!String.IsNullOrWhiteSpace (station))
        {
          fn += ("-" + station);
          if  (!String.IsNullOrWhiteSpace (deployment))
            fn += ("-" + deployment);
        }
      }
      
      if  (classToPlot != null)
        fn += ("_" + classToPlot.Name);
      fn += ("_" + cast);
      fn += ".txt";
      sfd.FileName = fn;

      sfd.InitialDirectory = lastSaveDirectory;

      //sfd.CheckFileExists = true;
      DialogResult dr = sfd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        fn = sfd.FileName;
        lastSaveDirectory = OSservices.GetPathPartOfFile (fn);

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


    private  void  WriteTabDelToStream (TextWriter                  tw,
                                        PicesImageSizeDistribution  sizeDistribution,
                                        bool                        printDensity
                                       )
    {

      uint x = 0;
      uint  numDepthBins = (uint)sizeDistribution.NumDepthBins;
      uint  numSizeBins  = (uint)sizeDistribution.NumSizeBuckets;

      float[]  startValues  = bucketsDisplayed.SizeStartValues ();
      float[]  endValues    = bucketsDisplayed.SizeEndValues   ();

      uint  maxValuesLen = (uint)Math.Max (startValues.Length, endValues.Length);
      uint  minValuesLen = (uint)Math.Min (startValues.Length, endValues.Length);

      String  hl1 = ""         + "\t"  + "Volume"       + "\t" + "Image" + "\t"  + "Total"  + "\t"  + "Total";
      String  hl2 = "Depth(m)" + "\t"  + "Sampled(m^3)" + "\t" + "Count" + "\t"  + "Pixels" + "\t"  + "FilledArea";


      for  (x = 0;  x < minValuesLen; ++x)
      {
        hl1 += ("\t" + startValues[x]);
        hl2 += ("\t" + endValues  [x]);
      }

      tw.WriteLine (hl1);
      tw.WriteLine (hl2);

      for  (uint depthBinIdx = 0;  depthBinIdx < numDepthBins;  ++depthBinIdx)
      {
        PicesImageSizeDistributionRow  row = sizeDistribution.GetDepthBin (depthBinIdx);
        if  (row == null)
          continue;

        uint[]  distriution = row.Distribution ();
        float   volumneSampled = row.VolumneSampled;

        tw.Write (row.Depth           + "\t" + 
                  volumneSampled      + "\t" +
                  row.ImageCount      + "\t" +
                  row.TotalPixels     + "\t" +
                  row.TotalFilledArea
                 );

        for  (x = 0;  x < distriution.Length;  ++x)
        {
          tw.Write ("\t");
          if  (printDensity)
          {
            float zed = 0.0f;
            if  ((distriution[x] > 0)  &&  (volumneSampled != 0.0f))
              zed  = (float)Math.Log10 (1.0f + (float)distriution[x] / (float)volumneSampled);
            tw.Write (zed);
          }
          else
          {
            tw.Write (distriution[x]);
          }
        }

        tw.WriteLine ();
      }
    }  /* WriteTabDelToStream */



    private  void  WriteTabDelToStream (TextWriter tw)
    {
      DateTime  curTime = DateTime.Now;
      tw.WriteLine ("ChartSizeDistribution"); 
      tw.WriteLine ("DateTime" + "\t" + curTime.ToString ("yyyy/MM/dd HH:mm:ss")); 
      tw.WriteLine ();
      tw.WriteLine ("Cruise"               + "\t" + cruise);
      tw.WriteLine ("Station"              + "\t" + station);
      tw.WriteLine ("Deployment"           + "\t" + deployment);
      tw.WriteLine ("Class"                + "\t" + classToPlot.Name);
      tw.WriteLine ("Include-Sub-Classes"  + "\t" + (includeSubClasses ? "Yes" : "No"));
      tw.WriteLine ("Cast"                 + "\t" + cast);
      tw.WriteLine ();
      tw.WriteLine ("There will be two tables below:  First: Counts   Second: Density.");

      tw.WriteLine ();
      tw.WriteLine ();
      tw.WriteLine ("Counts");
      tw.WriteLine ();
      WriteTabDelToStream (tw, bucketsDisplayed, false);
      
      tw.WriteLine ();
      tw.WriteLine ();
      tw.WriteLine ();
      tw.WriteLine ("Abundance (log10)");
      tw.WriteLine ();
      WriteTabDelToStream (tw, bucketsDisplayed, true);
      
      tw.WriteLine ();
      tw.WriteLine ("End of ChartSizeDistribution");
    }  /* WriteTabDelToStream */


    

    private  void  WriteForGnuplotToStream (TextWriter                  tw,
                                            PicesImageSizeDistribution  sizeDistribution,
                                            bool                        printDensity,
                                            bool                        plotVolumeSpectrum
                                           )
    {
      uint  numDepthBins = (uint)sizeDistribution.NumDepthBins;
      uint  numSizeBins  = (uint)sizeDistribution.NumSizeBuckets;

      float[]  startValues  = bucketsDisplayed.SizeStartValues ();
      float[]  endValues    = bucketsDisplayed.SizeEndValues   ();

      uint  maxValuesLen = (uint)Math.Max (startValues.Length, endValues.Length);
      uint  minValuesLen = (uint)Math.Min (startValues.Length, endValues.Length);

      double  depthBinSize = sizeDistribution.DepthBinSize;


      for  (int sizeIdx = 0;  sizeIdx < minValuesLen;  ++sizeIdx)
      {
        double  size = (startValues[sizeIdx] + endValues[sizeIdx]) / 2.0;
        for  (uint depthBinIdx = 0;  depthBinIdx < numDepthBins;  ++depthBinIdx)
        {
          double  depth = depthBinSize * depthBinIdx;
          PicesImageSizeDistributionRow  row = sizeDistribution.GetDepthBin (depthBinIdx);
          if  (row == null)
          { 
            tw.WriteLine (size + " " + depth + " " + "0");
            continue;
          }

          uint[]  distriution = row.Distribution ();
          float   volumneSampled = row.VolumneSampled;

          double  zed = 0.0;

          if  (plotVolumeSpectrum)
          {
            double  density = 0.0;
            if  ((distriution[sizeIdx] > 0)  &&  (volumneSampled != 0.0f))
              density  = (double)distriution[sizeIdx] / (double)volumneSampled;
            double  nV = density * Math.PI * Math.Pow (size, 3.0) / 6.0;
            double  nVd = nV * size;
            zed = Math.Log10 (nVd);
          }
          else
          {
            if  (printDensity)
            {
              if  ((distriution[sizeIdx] > 0)  &&  (volumneSampled != 0.0f))
                zed  = (float)Math.Log10 (1.0f + (float)distriution[sizeIdx] / (float)volumneSampled);
            }
            else
            {
              zed = (float)Math.Log10 (1.0 + (double)distriution[sizeIdx]);
            }
          }

          tw.WriteLine (size + " " + depth + " " + zed);
        }
        tw.WriteLine ();
      }
    }  /* WriteForGnuplotToStream */






    private  void  SaveForGnuplot (Object sender, EventArgs e)
    {
      SaveFileDialog sfd = new SaveFileDialog ();
      sfd.Filter = "Comma delimited data to disk|*.txt";
      sfd.Title = "Save Abundance info for gnuplot to disk.";
      sfd.DefaultExt = "txt";
      sfd.AddExtension = true;
      sfd.OverwritePrompt = true;

      String fn = cruise + "-" + station;
      if  (!String.IsNullOrEmpty (deployment))
        fn = fn + "-" + deployment;
      fn = fn + "_AbundanceByDeployment";
      if  (classToPlot != null)
        fn += ("_" + classToPlot.Name);
      fn += ".txt";
      sfd.FileName = fn;

      //sfd.CheckFileExists = true;
      DialogResult dr = sfd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        fn = sfd.FileName;

        System.IO.StreamWriter sw = null;
        try  {sw = new System.IO.StreamWriter (fn, false, System.Text.Encoding.ASCII);}
        catch (Exception) {sw = null;}

        if  (sw == null)
        {
          MessageBox.Show (this, "Could not open file[" + fn + "]", "Save for gnuplot", MessageBoxButtons.OK);
          return;
        }

        WriteForGnuplotToStream (sw, bucketsDisplayed, true, false);

        sw.Close ();
        sw = null;

        String  ext = OSservices.GetFileExtension (fn);
        String  ofn = OSservices.RemoveExtension (fn) + "_nVd." + ext;

        try  {sw = new System.IO.StreamWriter (ofn, false, System.Text.Encoding.ASCII);}
        catch (Exception) {sw = null;}

        if  (sw == null)
        {
          MessageBox.Show (this, "Could not open file[" + ofn + "]", "Save for gnuplot", MessageBoxButtons.OK);
          return;
        }

        WriteForGnuplotToStream (sw, bucketsDisplayed, true, true);

        sw.Close ();
        sw = null;

        MessageBox.Show (this, "Abundance Data for gnuplot written to \"" + fn + "\".", "", MessageBoxButtons.OK);
      }
    }  /* SaveForGnuplot */






    
    private void ChartSizeDistribution_FormClosing (object sender, FormClosingEventArgs e)
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
        m += "-" + d.DeploymentNum + "  " + d.Description;
      e.Value = m;
    }


    private void SelectClassButton_Click (object sender, EventArgs e)
    {
      SelectAPicesClass  sapc = new SelectAPicesClass (classes, rootDir, activeClasses);

      if  (!String.IsNullOrEmpty (ClassToPlot.Text))
      sapc.SelectedClass = PicesClassList.GetUniqueClass (ClassToPlot.Text, "");
      sapc.ShowDialog (this);
      if  (sapc.SelectionMade)
      {
        PicesClass pc = sapc.SelectedClass;
        if  (pc != null)
          ClassToPlot.Text = pc.Name;
      }
    }


    private  void  ClearHighLightedBucket ()
    {
      if  ((ProfileChart.Series.Count < 1)  ||  (selectedSizeBucket < 0))
        return;

      Series  s = ProfileChart.Series[0];
      if  (selectedSizeBucket >= s.Points.Count)
        return;

      s.Points[selectedSizeBucket].Label = "";
      s.Points[selectedSizeBucket].Color = s.Color;

      selectedSizeBucket = -1;
    }  /* ClearHighLightedBucket*/




    private  void  HighLightSizeBucket (int  sizeBucket)
    {
      if  ((ProfileChart.Series.Count < 1)  ||  (sizeBucket < 0))
        return;

      if (sizeBucket >= integratedDensityDistribution.Length)
        return;

      Series  s = ProfileChart.Series[0];
      if  (sizeBucket >= s.Points.Count)
        return;

      ClearHighLightedBucket ();

      DataPoint dp = s.Points[sizeBucket];
      double  d = (double)integratedDensityDistribution[sizeBucket];
      s.Points[sizeBucket].Label = d.ToString("###,##0.0") + "(" + dp.XValue.ToString("##0.000") + ")";
      s.Points[sizeBucket].Font = axisLabelFont;

      s.Points[sizeBucket].Color = Color.Red;

      selectedSizeBucket = sizeBucket;
    }  /* HighLightSizeBucket */




    private  void   ChartContextMenu (Point p, 
                                      int   sizeBucket
                                     )
    {
      MenuItem  i1 = new MenuItem ("Copy Chart to clipboard",              CopyChartToClipboard);
      MenuItem  i2 = new MenuItem ("Save Chart to Disk",                   SaveChartToDisk);
      MenuItem  i3 = new MenuItem ("Copy Data Tab-Delimited to Clipboard", SaveTabDelToClipBoard);
      MenuItem  i4 = new MenuItem ("Save Data Tab-Delimited to Disk",      SaveTabDelToDisk);
      MenuItem  i5 = new MenuItem ("Display Images for Size Range",        DisplayImagesForASizeBucket);
      MenuItem  i6 = new MenuItem ("Chart Vertical Distribution",          ChartVerticleDistributionForASizeBucket);
      MenuItem  i7 = new MenuItem ("Save data for 'gnuplot'",              SaveForGnuplot);

      MenuItem[] menuItems = null;
      if  (sizeBucket >= 0)
        menuItems = new MenuItem[]{i1, i2, i3, i4, i5, i6, i7};
      else
        menuItems = new MenuItem[]{i1, i2, i3, i4, i7};
      ContextMenu  buttonMenu = new ContextMenu (menuItems);
      buttonMenu.Show (ProfileChart, p);
    }  /* ChartContextMenu */


    private  void   DisplayImagesForASizeBucket (Object sender, EventArgs e)
    {
      if  ((selectedSizeBucket < 0)  ||  (bucketsDisplayed == null)   ||  (selectedSizeBucket >= bucketsDisplayed.NumSizeBuckets))
        return;

      float  sizeStart = 0.0f;
      float  sizeEnd   = 0.0f;
      uint   imageCount = 0;

      bucketsDisplayed.GetSizeBucketStats ((uint)selectedSizeBucket, ref imageCount, ref sizeStart, ref sizeEnd);

      char  castChar = 'B';
      if       (cast == "Down")  castChar = 'D';
      else if  (cast == "Up")    castChar = 'U';

      PicesDataBaseImageList  images =  mainWinConn.ImagesQueryDeploymentSizeRange (cruise, station, deployment, classToPlot, castChar, statistic, sizeStart, sizeEnd, 0.0f, 0.0f, 1000, true);
      
      DisplayPicesImages dpi = new DisplayPicesImages (mainWinConn, bucketsDisplayed, selectedSizeBucket, 0.0f, 0.0f, images);
      dpi.ShowDialog (this);
    }




    private  void   ChartVerticleDistributionForASizeBucket (Object sender, EventArgs e)
    {
      ChartSizeDepthDistribution  csdd = new ChartSizeDepthDistribution  
        (cruise, station, deployment, classToPlot, includeSubClasses, statistic, cast, bucketsDisplayed, selectedSizeBucket);
      csdd.ShowDialog (this);
    }



    private  void  ProfileChart_MouseClick (object sender, MouseEventArgs e)
    {
      if  ((ProfileChart.Series == null)  ||  (ProfileChart.Series.Count < 1))
        return;

      Series s = ProfileChart.Series[0];

      Point  p = e.Location;
      ChartArea  ca = ProfileChart.ChartAreas[0];
      double yValue = ca.AxisY.PixelPositionToValue ((double)p.Y);
      double xValue = ca.AxisX.PixelPositionToValue ((double)p.X);

      // Because the x-axis is Log10 of the actual value we need to raise 10 to the power of xValue to get the actual size.
      xValue = Math.Pow (10, xValue);

      int  sizeBucket = -1;
      if  (bucketsDisplayed != null)
        sizeBucket = bucketsDisplayed.IdentifySizeBucket ((float)xValue);

      if  (sizeBucket >= 0)
      {
        if  (sizeBucket >= s.Points.Count)
          sizeBucket = -1;
        else
        {
          double  y = s.Points[sizeBucket].YValues[0];
          if  ((yValue - y) > 0.3)
            sizeBucket = -1;
        }
      }

      if  (sizeBucket >= 0)
        HighLightSizeBucket (sizeBucket);

      if  (e.Button == System.Windows.Forms.MouseButtons.Right)
        ChartContextMenu (p, sizeBucket); 
    }  /* ProfileChart_MouseClick */
  }
}
