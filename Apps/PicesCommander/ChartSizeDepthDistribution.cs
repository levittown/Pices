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
  public partial class ChartSizeDepthDistribution : Form
  {
    private  PicesRunLog      runLog     = null;
    private  PicesDataBase    mainWinConn = null;
    private  PicesDataBase    threadConn  = null;  /**< This connection will only be use by the background thread. */

    private  PicesClass         classToPlot        = null;
    private  bool               includeSubClasses  = false;
    private  String             cruise             = null;
    private  String             station            = null;
    private  String             deployment         = null;
    private  String             cast               = "Down";
    private  char               statistic          = '0';
    private  String             statisticStr       = "Area mm^2";
    private  float              sizeStart          = 0.0f;
    private  float              sizeEnd            = 0.0f;
    private  int                selectedSizeBucket = -1;
    PicesImageSizeDistribution  sizeDistribution   = null;
    private  float[]            sizeStartValues    = null;
    private  float[]            sizeEndValues      = null;


    private  PicesGoalKeeper  goalie = new PicesGoalKeeper ("ChartSizeDepthDistribution");

    private  int  heightLast = 0;
    private  int  widthLast  = 0;

    private  Color[]  colorTable = {Color.Black,      Color.Red,        Color.Green,      
                                    Color.Blue,       Color.Crimson,    Color.DarkCyan,   
                                    Color.DarkOrange, Color.Lime,       Color.SeaGreen
                                   };

    private  bool    formIsMaximized = false;


    public ChartSizeDepthDistribution (String                      _cruise,
                                       String                      _station,
                                       String                      _deployment,
                                       PicesClass                  _classToPlot,
                                       bool                        _includeSubClasses,
                                       char                        _statistic,
                                       String                      _cast,
                                       PicesImageSizeDistribution  _sizeDistribution,
                                       int                         _selectedSizeBucket
                                      )
    {
      cruise             = _cruise;
      station            = _station;
      deployment         = _deployment;
      classToPlot        = _classToPlot;
      includeSubClasses  = _includeSubClasses;
      cast               = _cast;
      statistic          = _statistic;
      sizeDistribution   = _sizeDistribution;
      selectedSizeBucket = _selectedSizeBucket;
      sizeStartValues    = sizeDistribution.SizeStartValues ();
      sizeEndValues      = sizeDistribution.SizeEndValues ();

      if  ((selectedSizeBucket >= 0)  &&  (selectedSizeBucket < sizeDistribution.NumSizeBuckets))
      {
        sizeStart = sizeStartValues[selectedSizeBucket];
        sizeEnd   = sizeEndValues  [selectedSizeBucket];
      }

      runLog     = new PicesRunLog ();

      mainWinConn = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);

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


      DeploymentField.Text = cruise + "-" + station + "-" + deployment;

      if  (classToPlot != null)
        ClassToPlot.Text = classToPlot.Name;

      SizeRangeField.Text = sizeStart.ToString ("##0.000") + " - " + sizeEnd.ToString ("##0.000");
      
      switch  (statistic)
      {
      case  '0':  statisticStr = "Area mm^2";       break;
      case  '1':  statisticStr = "Diameter (ESD)";  break;
      case  '2':  statisticStr = "Volume (EBv)";    break;
      }

      SizeStaisticField.Text = statisticStr;

      CastField.Text = cast;
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
      goalie.StartBlock ();

      Font titleFont     = new Font (FontFamily.GenericSansSerif, 12);
      Font axisLabelFont = new Font (FontFamily.GenericSansSerif, 12);

      String  t1 = "Abundance by Size";
      switch  (statistic)
      {
      case '0': t1 = "Abundance by Size";                      break;
      case '1': t1 = "Abundance by Diamter (ESD)";             break;
      case '2': t1 = "Abundance by Volume (EBv";               break;
      case '3': t1 = "Abundance by Estimated Elipsoid Volume"; break;
      }

      String  t2 = "Cruise: " + cruise + "  Station: " + station;
      if  (String.IsNullOrEmpty (deployment))
        t2 += "  Deployment: " + deployment;
      t2 += "  Class: " + classToPlot.Name;

      String t3 = statisticStr + " - " + cast + "-Cast";



      ProfileChart.Titles.Clear ();
      ProfileChart.Titles.Add (new Title (t1, Docking.Top, titleFont, Color.Black));
      ProfileChart.Titles.Add (new Title (t2, Docking.Top, titleFont, Color.Black));
      ProfileChart.Titles.Add (new Title (t3, Docking.Top, titleFont, Color.Black));

      ProfileChart.Series.Clear ();

      ChartArea ca = ProfileChart.ChartAreas[0];

      Series s = new Series ("Size Distribution");
      s.ChartArea = "ChartArea1";
      s.ChartType = SeriesChartType.Column;


      /*
      float  minX = float.MaxValue;
      float  minY = float.MaxValue;
      float  maxX = float.MinValue;
      float  maxY = float.MinValue;

      int  numDepthBins = sizeDistribution.NumDepthBins;
      
      for  (int depthBin = 0;  depthBin < numDepthBins;  ++depthBin)
      {
        sizeDistribution.
        float  sv = startValues[];
        float  ev = sv * growthRate;
        float  midPoint = (sv +ev) / 2.0f;
        if  (sv <= 0.0f)
          midPoint = initialSizeValue / 2.0f;

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

      ca.AxisY.Title = SubInSuperScriptExponent ("Log 10  Abundance");

      switch  (statistic)
      {
      case '0':
        ca.AxisX.Title = SubInSuperScriptExponent ("area(m-2)");
        ca.AxisX.LabelStyle.Format =  "##,##0.000";
        break;

      case '1':
        ca.AxisX.Title = SubInSuperScriptExponent ("Length(mili-meters)");
        ca.AxisX.LabelStyle.Format =  "##00.00";
        break;

      case '2':
        ca.AxisX.Title = SubInSuperScriptExponent ("Volume(mm-3)");
        ca.AxisX.LabelStyle.Format =  "##0.0000";
        break;

      case '3':
        ca.AxisX.Title = SubInSuperScriptExponent ("Volume(mm-3)");
        ca.AxisX.LabelStyle.Format =  "##0.0000";
        break;
      }

      ca.AxisX.IsLogarithmic    = true;
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
      */

      goalie.EndBlock ();
    }  /* UpdateChartAreas */




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
        // the ResizeEnd envent does not trap when form is Maximized.
        //PicesCommanderFormResized ();
        ChartAbundanceByDeployment_Resize (sender, e);
        formIsMaximized = true;
      }
      else if  (WindowState == FormWindowState.Normal)
      {
        if  (formIsMaximized)
        {
          // We normally trap the ResizeEnd event;  but when the form was already maximized and the user
          // presses the button to unmaximize.  the ResizeEnd does not trap that.  So we check to 
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

      String fn = cruise + "_AbundanceByDeployment";
      if  (classToPlot != null)
        fn += ("_" + classToPlot.Name);
      fn += ".txt";
      sfd.FileName = fn;

      //sfd.CheckFileExists = true;
      DialogResult dr = sfd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        fn = sfd.FileName;
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
      tw.WriteLine ("ChartSizeDepthDistribution"); 
      tw.WriteLine ("DateTime" + "\t" + curTime.ToString ("yyyy/MM/dd HH:mm:ss")); 
      tw.WriteLine ();
      tw.WriteLine ("Cruise"               + "\t" + cruise);
      tw.WriteLine ("Station"              + "\t" + station);
      tw.WriteLine ("Deployment"           + "\t" + deployment);
      tw.WriteLine ("Class"                + "\t" + classToPlot.Name);
      tw.WriteLine ("Include-Sub-Classes"  + "\t" + (includeSubClasses ? "Yes" : "No"));
      tw.WriteLine ();
      tw.WriteLine ("There will be two tables below: 1-Density, 2-Counts.");
      tw.WriteLine ();
      tw.WriteLine ();
      tw.WriteLine ();

      tw.WriteLine ("Density");
      tw.WriteLine ();
      int  largestIdx = 0;
      tw.Write ("Depth" + "\t" + "Vol-Sampled");
      /*
      foreach (DataSeriesToPlot dstp in series)
      {
        String s = dstp.legend;
        tw.Write ("\t" + s);
        if  (dstp.density.Length > largestIdx)
          largestIdx = dstp.density.Length;
      }
      tw.WriteLine ();

      for  (int  depthIdx = 0;  depthIdx < largestIdx;  ++depthIdx)
      {
        tw.Write (depthIdx * depthIncrement);
        tw.Write ("\t");
        if  (depthIdx < depthVolumeProfile.Length)
          tw.Write (depthVolumeProfile[depthIdx]);

        foreach  (DataSeriesToPlot dstp in series)
        {
          tw.Write ("\t");
          if  (depthIdx < dstp.density.Length)
            tw.Write (dstp.density[depthIdx]);
        }
        tw.WriteLine ();
      }

      tw.WriteLine ();
      tw.WriteLine ();
      tw.WriteLine ();

      tw.WriteLine ("Counts");
      tw.WriteLine ();
      largestIdx = 0;
      tw.Write ("Depth" + "\t" + "Vol-Sampled");
      foreach (DataSeriesToPlot dstp in series)
      {
        String s = dstp.legend;
        tw.Write ("\t" + s);
        if  (dstp.countsByDepth.Length > largestIdx)
          largestIdx = dstp.countsByDepth.Length;
      }
      tw.WriteLine ();

      for  (int  depthIdx = 0;  depthIdx < largestIdx;  ++depthIdx)
      {
        tw.Write (depthIdx * depthIncrement);

        tw.Write ("\t");
        if  (depthIdx < depthVolumeProfile.Length)
          tw.Write (depthVolumeProfile[depthIdx]);
        
        foreach  (DataSeriesToPlot dstp in series)
        {
          tw.Write ("\t");
          if  (depthIdx < dstp.countsByDepth.Length)
            tw.Write (dstp.countsByDepth[depthIdx]);
        }
        tw.WriteLine ();
      }
      */

      tw.WriteLine ();
      tw.WriteLine ("End of ChartSizeDepthDistribution");
    }  /* WriteTabDelToStream */


    private void ChartSizeDepthDistribution_FormClosing (object sender, FormClosingEventArgs e)
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

      if  (sizeBucket >= sizeDistribution.NumDepthBins)
        return;

      Series  s = ProfileChart.Series[0];
      if  (sizeBucket >= s.Points.Count)
        return;

      ClearHighLightedBucket ();

      DataPoint dp = s.Points[sizeBucket];
      double  d = (double)integratedDensityDistribution[sizeBucket];
      s.Points[sizeBucket].Label = d.ToString("###,##0.0") + "(" + dp.XValue.ToString("##0.000") + ")";
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
      MenuItem  i6 = new MenuItem ("Chart Verticle Distribution",          ChartVerticleDistributionForASizeBucket);
      MenuItem[] menuItems = null;
      if  (sizeBucket >= 0)
        menuItems = new MenuItem[]{i1, i2, i3, i4, i5, i6};
      else
        menuItems = new MenuItem[]{i1, i2, i3, i4};
      ContextMenu  buttonMenu = new ContextMenu (menuItems);
      buttonMenu.Show (ProfileChart, p);
    }


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
      
      DisplayPicesImages dpi = new DisplayPicesImages (mainWinConn, bucketsDisplayed, selectedSizeBucket, images);
      dpi.ShowDialog (this);
    }

    private  void   ChartVerticleDistributionForASizeBucket (Object sender, EventArgs e)
    {
      //
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

      // Because the x-axis is Log10 of the actiual value we need to raise 10 to the power of xValue to get the actual size.
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
