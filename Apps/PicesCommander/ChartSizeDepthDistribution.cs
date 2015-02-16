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
    private  PicesRunLog        runLog             = null;
    private  PicesDataBase      mainWinConn        = null;
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
    private  float[]            depthProfile       = null;
    private  float[]            volumeSampled      = null;
    private  int                selectedDepthBin   = -1;   //  The currently selected depth column.

    Font titleFont     = new Font (FontFamily.GenericSansSerif, 12);
    Font axisLabelFont = new Font (FontFamily.GenericSansSerif, 12);
    Font dataPointFont = new Font (FontFamily.GenericSansSerif, 12);


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
      depthProfile       = sizeDistribution.DepthProfileForSizeBin ((uint)selectedSizeBucket);
      volumeSampled      = sizeDistribution.VolumeSampledByDepthBucket ();

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

      PlotLogDensity.Checked = true;

      switch  (statistic)
      {
      case  '0':  statisticStr = "Area mm^2";       break;
      case  '1':  statisticStr = "Diameter (ESD)";  break;
      case  '2':  statisticStr = "Volume (EBv)";    break;
      }

      SizeStaisticField.Text = statisticStr;

      CastField.Text = cast;

      UpdateChartAreas ();
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

      bool  plotLogDensity = PlotLogDensity.Checked;

      String  t1 = "Abundance by Size";
      switch  (statistic)
      {
      case '0': t1 = "Abundance by Size";                      break;
      case '1': t1 = "Abundance by Diamter (ESD)";             break;
      case '2': t1 = "Abundance by Volume (EBv";               break;
      case '3': t1 = "Abundance by Estimated Elipsoid Volume"; break;
      }

      String  t2 = cruise + "-" + station;
      if  (!String.IsNullOrEmpty (deployment))
        t2 += ("-" + deployment);
      t2 += ("  " + classToPlot.Name);
      t2 += "  ";
      t2 += (cast + "-Cast"  +  "  Size-Range(" + sizeStart.ToString () + " - " + sizeEnd.ToString () + ")");

      ProfileChart.Titles.Clear ();
      ProfileChart.Titles.Add (new Title (t1, Docking.Top, titleFont, Color.Black));
      ProfileChart.Titles.Add (new Title (t2, Docking.Top, titleFont, Color.Black));
      //ProfileChart.Titles.Add (new Title (t3, Docking.Top, titleFont, Color.Black));

      ProfileChart.Series.Clear ();

      ChartArea ca1 = ProfileChart.ChartAreas[0];
      ChartArea ca2 = ProfileChart.ChartAreas[1];

      Series s = new Series ("Size Distribution");
      s.ChartArea = "ChartArea1";
      s.ChartType = SeriesChartType.Line;

      Series s2 = new Series ("Volumne Sampled");
      s2.ChartArea = "ChartArea2";
      s2.ChartType = SeriesChartType.Line;

      float  minX = float.MaxValue;
      float  minY = float.MaxValue;
      float  maxX = float.MinValue;
      float  maxY = float.MinValue;

      int  numDepthBins = sizeDistribution.NumDepthBins;
      for  (uint depthBin = 0;  depthBin < numDepthBins;  ++depthBin)
      {
        PicesImageSizeDistributionRow  row = sizeDistribution.GetDepthBin (depthBin);
        if  (row == null)
          continue;
        float  depth = row.Depth;
        double density = 0.0;
        if  (depthBin < depthProfile.Length)
           density = (double)depthProfile[depthBin];

        if  (plotLogDensity)
          density = (float)Math.Log10 (density + 1.0);

        minX = Math.Min (minX, (float)density);
        maxX = Math.Max (maxX, (float)density);
        minY = Math.Min (minY, depth);
        maxY = Math.Max (maxY, depth);
        DataPoint dp = new DataPoint (density, depth);
        s.Points.Add (dp);

        double  vs = 0.0;
        if  (depthBin < volumeSampled.Length)
          vs = volumeSampled[depthBin];
        DataPoint dp2 = new DataPoint (vs, depth);
        s2.Points.Add (dp2);
      }
      s.XAxisType = AxisType.Primary;
      s.YAxisType = AxisType.Primary;
      s2.XAxisType = AxisType.Primary;
      s2.YAxisType = AxisType.Primary;
      s2.Color = Color.Black;

      if  (plotLogDensity)
        ca1.AxisX.Title = SubInSuperScriptExponent ("Log 10  Abundance");
      else
        ca1.AxisX.Title = SubInSuperScriptExponent ("Abundance");

      ca1.AxisY.Title = SubInSuperScriptExponent ("Depth (Meters)");
      ca1.AxisY.LabelStyle.Format =  "##,##0";

      ca1.AxisX.IsLogarithmic     = false;
      ca1.AxisX.Minimum           = 0.0;
      //ca1.AxisX.Maximum           = maxX;
      ca1.AxisX.LabelStyle.Angle  = 90;
      ca1.AxisX.LabelStyle.Font   = axisLabelFont;
      ca1.AxisX.TitleFont         = axisLabelFont;

      ca1.AxisY.Minimum           = 0.0;
      ca1.AxisY.IsLogarithmic     = false;
      ca1.AxisY.LabelStyle.Font   = axisLabelFont;
      ca1.AxisY.TitleFont         = axisLabelFont;
      if  (plotLogDensity)
        ca1.AxisX.LabelStyle.Format =  "##,##0.0";
      else
        ca1.AxisX.LabelStyle.Format =  "##,##0";

      ca2.AxisX.Title = SubInSuperScriptExponent ("Volume Sampled (mm-3)");
      ca2.AxisX.Minimum = 0.0;
      ca2.AxisX.IsLogarithmic     = false;
      ca2.AxisX.LabelStyle.Font   = axisLabelFont;
      ca2.AxisX.TitleFont         = axisLabelFont;
      ca2.AxisX.LabelStyle.Format =  "##0.00";
      ca2.AxisX.LabelStyle.Angle  = 90;

      s.BorderWidth = 2;
      ProfileChart.Series.Add (s);
      ProfileChart.Series.Add (s2);

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




    private void ChartAbundanceByDeployment_Resize (object sender, EventArgs e)
    {
      int  heightDelta = Height - heightLast;
      int  widthDelta  = Width  - widthLast;

      ProfileChart.Height += heightDelta;
      ProfileChart.Width  += widthDelta;

   
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
      mainWinConn.Close ();
      mainWinConn = null;
      GC.Collect ();
    }



    private  void  ClearHighLightedBucket ()
    {
      if  ((ProfileChart.Series.Count < 1)  ||  (selectedDepthBin < 0))
        return;

      Series  s = ProfileChart.Series[0];
      if  (selectedDepthBin < s.Points.Count)
      {
        DataPoint dp = s.Points[selectedDepthBin];
        dp.Label       = "";
        dp.Color       = s.Color;
        dp.MarkerStyle = s.MarkerStyle;
        dp.MarkerSize  = s.MarkerSize;
        dp.MarkerColor = s.MarkerColor;
      }

      Series  s2 = ProfileChart.Series[1];
      if  (selectedDepthBin < s2.Points.Count)
      {
        DataPoint dp2  = s2.Points[selectedDepthBin];
        dp2.Label       = "";
        dp2.Color       = s2.Color;
        dp2.MarkerStyle = s2.MarkerStyle;
        dp2.MarkerSize  = s2.MarkerSize;
        dp2.MarkerColor = s2.MarkerColor;
      }
      selectedDepthBin = -1;
    }  /* ClearHighLightedBucket*/




    private  void  HighLightSizeBucket (int  depthBin)
    {
      if  ((ProfileChart.Series.Count < 1)  ||  (depthBin < 0))
        return;

      if  (depthBin >= depthProfile.Length)
        return;

      Series  s = ProfileChart.Series[0];
      if  (depthBin >= s.Points.Count)
        return;

      ClearHighLightedBucket ();

      DataPoint dp = s.Points[depthBin];

      PicesImageSizeDistributionRow  depthRow = sizeDistribution.GetDepthBin ((uint)depthBin);
      if  (depthRow == null)
        return;
      uint[] distribution = depthRow.Distribution ();
      if  ((distribution == null)  ||  (distribution.Length <= selectedSizeBucket))
        return;

      double  volSampled = 0.0f;
      if  (depthBin < volumeSampled.Length)
        volSampled = volumeSampled[depthBin];
      double  density = 0.0;
      if  (volSampled > 0.0)
        density = (double)distribution[selectedSizeBucket] / volSampled;

      dp.Label = density.ToString ("###,##0.0") + "(" + dp.YValues[0].ToString ("##0.0 m") + ")";
      dp.Color = Color.Red;
      dp.Font = dataPointFont;
      dp.MarkerStyle = MarkerStyle.Circle;
      dp.MarkerSize = s.MarkerSize * 3;
      dp.MarkerColor = Color.Red;
      dp.LabelBackColor = Color.White;

      Series s2 = ProfileChart.Series[1];
      if  (depthBin >= s2.Points.Count)
        return;

      DataPoint dp2 = s2.Points[depthBin];
      dp2.Label = volSampled.ToString ("#0.00") + "m^3";
      dp2.Color = Color.Red;
      dp2.Font = dataPointFont;
      dp2.MarkerStyle = MarkerStyle.Circle;
      dp2.MarkerSize = s.MarkerSize * 3;
      dp2.MarkerColor = Color.Red;
      dp2.LabelBackColor = Color.White;

      selectedDepthBin = depthBin;
    }  /* HighLightSizeBucket */




    private  void   ChartContextMenu (Point p, 
                                      int   depthBin
                                     )
    {
      MenuItem  i1 = new MenuItem ("Copy Chart to clipboard",              CopyChartToClipboard);
      MenuItem  i2 = new MenuItem ("Save Chart to Disk",                   SaveChartToDisk);
      MenuItem  i3 = new MenuItem ("Copy Data Tab-Delimited to Clipboard", SaveTabDelToClipBoard);
      MenuItem  i4 = new MenuItem ("Save Data Tab-Delimited to Disk",      SaveTabDelToDisk);
      MenuItem  i5 = new MenuItem ("Display Images for Size Range",        DisplayImagesForDepthBin);
      MenuItem[] menuItems = null;
      if  (depthBin >= 0)
        menuItems = new MenuItem[]{i1, i2, i3, i4, i5};
      else
        menuItems = new MenuItem[]{i1, i2, i3, i4};
      ContextMenu  buttonMenu = new ContextMenu (menuItems);
      buttonMenu.Show (ProfileChart, p);
    }




    private  void   DisplayImagesForDepthBin (Object sender, EventArgs e)
    {
      if  ((selectedDepthBin < 0)  ||  (sizeDistribution == null)   ||  (selectedDepthBin >= sizeDistribution.NumDepthBins))
        return;

      float  sizeStart = 0.0f;
      float  sizeEnd   = 0.0f;
      uint   imageCount = 0;

      sizeDistribution.GetSizeBucketStats ((uint)selectedSizeBucket, ref imageCount, ref sizeStart, ref sizeEnd);

      char  castChar = 'B';
      if       (cast == "Down")  castChar = 'D';
      else if  (cast == "Up")    castChar = 'U';

      float  depthMin = selectedDepthBin * sizeDistribution.DepthBinSize;
      float  depthMax = depthMin + sizeDistribution.DepthBinSize;

      PicesDataBaseImageList  images =  mainWinConn.ImagesQueryDeploymentSizeRange (cruise, station, deployment, classToPlot, castChar, statistic, sizeStart, sizeEnd, depthMin, depthMax, 1000, true);
      
      DisplayPicesImages dpi = new DisplayPicesImages (mainWinConn, sizeDistribution, selectedSizeBucket, depthMin, depthMax, images);
      dpi.ShowDialog (this);
    }





    private  void  ProfileChart_MouseClick (object sender, MouseEventArgs e)
    {
      if  ((ProfileChart.Series == null)  ||  (ProfileChart.Series.Count < 1)  ||  (sizeDistribution == null))
        return;

      Series s = ProfileChart.Series[0];

      Point  p = e.Location;
      ChartArea  ca = ProfileChart.ChartAreas[0];
      double yValue = ca.AxisY.PixelPositionToValue ((double)p.Y);
      double xValue = ca.AxisX.PixelPositionToValue ((double)p.X);

      int  depthBin = -1;
      depthBin = (int)(yValue / sizeDistribution.DepthBinSize);

      if  (depthBin >= 0)
      {
        if  (depthBin >= depthProfile.Length)
          depthBin = -1;
        else
        {
          double  x = s.Points[depthBin].YValues[0];
          //if  (Math.Abs (xValue - x) > 0.6)
          //  depthBin = -1;
        }
      }

      if  (depthBin >= 0)
        HighLightSizeBucket (depthBin);

      if  (e.Button == System.Windows.Forms.MouseButtons.Right)
        ChartContextMenu (p, depthBin); 
    }

    private void PlotLogDensity_CheckedChanged (object sender, EventArgs e)
    {
      UpdateChartAreas ();
    }  /* ProfileChart_MouseClick */
  }
}
