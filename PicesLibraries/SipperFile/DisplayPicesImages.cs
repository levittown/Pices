using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

using  PicesInterface;

namespace SipperFile
{
  public partial class DisplayPicesImages : Form
  {
    private  String                dir;
    private  String                dir2 = null;
    private  ProbNamePair[]        nameList = null;

    private  int                   lastImageIndexLoaded = 0;
    private  FileInfo[]            files = null;
    private  List<FlowLayoutPanel> thumbNails = null;

    private  int                   lastHeight = 0;
    private  int                   lastWidth  = 0;

    private  String[]              svNames = null;

    /*  These next four variables will be used when displaying images that are in the Pices database.  */
    PicesDataBase                  dbConn = null;
    PicesDataBaseImageList         picesImages = null;
    PicesImageSizeDistribution     sizeDistribution = null;
    int                            selectedSizeIndex = 0;
    float                          depthMin = 0.0f;
    float                          depthMax = 0.0f;

    String                         lastSaveImageDir = null;


    public DisplayPicesImages (String  _dir)
    {
      InitializeComponent ();
      
      dir = OSservices.AddSlash (_dir);
      Text = dir;

      thumbNails = new List<FlowLayoutPanel> ();
      
      DirectoryInfo di = null;
      try  {di = new DirectoryInfo (dir);}
      catch (Exception)  {di = null;}
      if  (di == null)
        return;

      try  {files = di.GetFiles ("*.*");}  catch (Exception)  {files = null;}
      if  (files != null)
      {
        files = ReduceToImageFiles (files);
        UpdateDisplayTimer.Enabled = true;
      }
    }


    public DisplayPicesImages (String          _dir,
                               String          _dir2,
                               ProbNamePair[]  _nameList
                              )
    {
      InitializeComponent ();
      
      dir  = OSservices.AddSlash (_dir);
      dir2 = OSservices.AddSlash (_dir2);

      nameList = _nameList;

      Text = dir;

      if  (!String.IsNullOrEmpty (dir)  &&  !String.IsNullOrEmpty (dir2))
        Text = OSservices.GetRootNameOfDirectory (dir) + " vs " + OSservices.GetRootNameOfDirectory (dir2);

      thumbNails = new List<FlowLayoutPanel> ();
      
      DirectoryInfo di = null;
      try  {di = new DirectoryInfo (dir);}
      catch (Exception)  {di = null;}
      if  (di == null)
        return;

      try  {files = di.GetFiles ("*.*");}  catch (Exception)  {files = null;}
      files = ReduceToImageFiles (files);
      if  (files != null)
        UpdateDisplayTimer.Enabled = true;
    }
    


    public DisplayPicesImages (PicesDataBase               _dbConn,
                               PicesImageSizeDistribution  _sizeDistribution,
                               int                         _selectedSizeIndex,
                               float                       _depthMin,
                               float                       _depthMax,
                               PicesDataBaseImageList      _picesImages
                              )
    {
      InitializeComponent ();
      thumbNails = new List<FlowLayoutPanel> ();

      dbConn            = _dbConn;
      sizeDistribution  = _sizeDistribution;
      picesImages       = _picesImages;
      selectedSizeIndex = _selectedSizeIndex;
      depthMin          = _depthMin;
      depthMax          = _depthMax;

      float[] startValues = sizeDistribution.SizeStartValues ();
      float[] endValues    = sizeDistribution.SizeEndValues ();

      if  ((selectedSizeIndex >= 0)  &&  (selectedSizeIndex < startValues.Length))
      {
        String  t = "Size Range [" + startValues[selectedSizeIndex].ToString ("###,##0.000") + " - " + endValues[selectedSizeIndex].ToString ("#,###,##0.000") + "]";
        if (depthMax > 0.0f)
          t = t + "   Depth Range [" + depthMin.ToString ("##0.0") + " - " + depthMax.ToString ("##0.0") + "]";
        Text = t;
      }

      UpdateDisplayTimer.Enabled = true;
    }



    private  FileInfo[]  ReduceToImageFiles (FileInfo[]  src)
    {
      List<FileInfo>  result = new List<FileInfo> ();
      foreach  (FileInfo fi in src)
      {
        if  (fi.Extension.Equals (".jpg", StringComparison.OrdinalIgnoreCase)  ||
             fi.Extension.Equals (".bmp", StringComparison.OrdinalIgnoreCase)
            )
          result.Add (fi);
      }
      return result.ToArray ();
    }  /* ReduceToImageFiles */



    /*! 
     *\brief  Specify the root file names of S/V's.  They will be highlighted when displayed.
     */
    public  void  SummportVectorNames (String[]  _svNames)
    {
      svNames = _svNames;
    }



    private  bool  IsIt_A_SV (String  n)
    {
      if  ((svNames == null)  ||  (n == null))
        return  false;
      foreach  (String  s in svNames)
      {
        if  (s.CompareTo (n) == 0)
          return true;
      }
      return  false;
    }



    public bool GetThumbnailImageAbort()
    {
      return true;
    }  /* GetThumbnailImageAbort */



    private  System.IntPtr  thumbNailCallBackData = IntPtr.Zero;
    


    private  void  LoadNextImageFromPicesList ()
    {
      if  (picesImages == null)
        return;

      if  (lastImageIndexLoaded >= picesImages.Count)
      {
        UpdateDisplayTimer.Enabled = false;
        return;
      }

      Font font = new System.Drawing.Font("Courier New", 8.0f, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));

      Image  image = null;
      
      PicesDataBaseImage  pi = picesImages[lastImageIndexLoaded];
      lastImageIndexLoaded++;

      image = pi.Thumbnail ();
      if  (image == null)
         return;

      float  ratio = 1.0f;
      int  maxDim = Math.Max (image.Height, image.Width);
      if  (maxDim > 150)
         ratio = 150.0f / (float)maxDim;

      PictureBox pb = new PictureBox ();
      pb.BorderStyle = BorderStyle.FixedSingle;

      int h = (int)((float)image.Height * ratio);
      int w = (int)((float)image.Width  * ratio);

      pb.Height = h + 2;
      pb.Width  = w + 2;
      //pb.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(PicesThumbNail_MouseDoubleClick);
      pb.MouseClick += new MouseEventHandler (MouseClickEvent);
      pb.Name = pi.ImageFileName;

      Image thumbNail = image.GetThumbnailImage (w, h, GetThumbnailImageAbort, thumbNailCallBackData);
      pb.Image = thumbNail;

      FlowLayoutPanel pan = new FlowLayoutPanel ();
      pan.Size = new Size (Math.Max (170, w + 10), 200);
      //pan.Height = h + 80;
      //pan.Width  = Math.Max (160, w + 10);

      String  rootName = OSservices.GetRootName (pi.ImageFileName);
      
      pan.Controls.Add (pb);
      TextBox tb = new TextBox ();
      tb.Width = pan.Width - 8;
      tb.Text = "Depth " + pi.Depth.ToString ("##0.0");
      tb.Font = font;
      pan.Controls.Add (tb);

      pan.BorderStyle = BorderStyle.FixedSingle;

      pan.BackColor = Color.White;
      
      thumbNails.Add (pan);
      ImageDisplayPanel.Controls.Add (pan);
    }  /* LoadNextImageFromPicesList */



    private EventHandler removeImageFromTrainigLibrayEventHandler = null;



    private  void  LoadNextImageFromDir ()
    {
      if  (files == null)
        return;

      if (removeImageFromTrainigLibrayEventHandler == null)
        removeImageFromTrainigLibrayEventHandler = new EventHandler(RemoveImageFromTrainigLibray);

      if  (lastImageIndexLoaded >= files.Length)
      {
        UpdateDisplayTimer.Enabled = false;
        return;
      }

      Font font = new System.Drawing.Font("Courier New", 8.0f, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));

      Bitmap  image = null;
      
      FileInfo fi = files[lastImageIndexLoaded];
      lastImageIndexLoaded++;

      try  {image = new Bitmap (fi.FullName);}
      catch (Exception) {image = null;}
      if  (image == null)
         return;

      float  ratio = 1.0f;
      int  maxDim = Math.Max (image.Height, image.Width);
      if  (maxDim > 150)
         ratio = 150.0f / (float)maxDim;

      PictureBox pb = new PictureBox ();
      pb.BorderStyle = BorderStyle.FixedSingle;

      int h = (int)((float)image.Height * ratio);
      int w = (int)((float)image.Width  * ratio);

      pb.Height = h + 2;
      pb.Width  = w + 2;
      pb.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(ThumbNail_MouseDoubleClick);
      pb.Name = fi.FullName;

      Image thumbNail = image.GetThumbnailImage (w, h, GetThumbnailImageAbort, thumbNailCallBackData);
      pb.Image = thumbNail;

      FlowLayoutPanel pan = new FlowLayoutPanel ();
      pan.Size = new Size (Math.Max (170, w + 10), 200);
      //pan.Height = h + 80;
      //pan.Width  = Math.Max (160, w + 10);

      String  rootName = OSservices.GetRootName (fi.Name);
      
      pan.Controls.Add (pb);
      TextBox tb = new TextBox ();
      tb.Width = pan.Width - 8;
      tb.Text = rootName;
      tb.Font = font;

      pan.Controls.Add (tb);

      Button b = new Button ();
      b.Text = "Delete";
      b.Name = rootName;
      b.Enabled = true;
      b.TabStop = false;
      b.Click += removeImageFromTrainigLibrayEventHandler;
      pan.Controls.Add (b);

      pan.BorderStyle = BorderStyle.FixedSingle;

      if  (IsIt_A_SV (rootName))
        pan.BackColor = Color.Gold;
      else
        pan.BackColor = Color.White;
      
      thumbNails.Add (pan);
      ImageDisplayPanel.Controls.Add (pan);
    }  /* LoadNextImageFromDir */



    private  String  LocateRootName (String  rootName)
    {
      String  subDir = null;
      if  (!String.IsNullOrEmpty (dir))
        subDir = OSservices.LookForFile (rootName + ".bmp", dir);

      if  (String.IsNullOrEmpty (subDir))
      {
        if  (!String.IsNullOrEmpty (dir2))
        {
          subDir = OSservices.LookForFile (rootName + ".bmp", dir2);
          if  (subDir == null)
            return  null;
        }
      } 

      return  OSservices.AddSlash (subDir) + rootName + ".bmp";
    }  /* LocateRootName */



    private  void  LoadNextImageFromNameList ()
    {
      if  (nameList == null)
        return;

      if  (lastImageIndexLoaded >= nameList.Length)
      {
        UpdateDisplayTimer.Enabled = false;
        return;
      }

      String  rootName    = nameList[lastImageIndexLoaded].name;
      double  probability = nameList[lastImageIndexLoaded].probability;

      lastImageIndexLoaded++;

      String  fullName = LocateRootName (rootName);
      if (fullName == null)
        return;

      String  subDir = OSservices.GetPathPartOfFile (fullName);

      FileInfo fi = new FileInfo (fullName);

      Bitmap  image = null;

      try  {image = new Bitmap (fi.FullName);}
      catch (Exception) {image = null;}
      if  (image == null)
         return;

      float  ratio  = 1.0f;
      int  maxDim = Math.Max (image.Height, image.Width);
      if  (maxDim > 150)
        ratio = 150.0f / (float)maxDim;

      PictureBox pb = new PictureBox ();
      pb.BorderStyle = BorderStyle.FixedSingle;

      int h = (int)((float)image.Height * ratio);
      int w = (int)((float)image.Width  * ratio);

      Font font = new System.Drawing.Font("Courier New", 8.0f, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));

      pb.Height = h + 2;
      pb.Width  = w + 2;
      pb.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(ThumbNail_MouseDoubleClick);
      pb.Name = fi.FullName;

      //ContextMenuStrip cms = new ContextMenuStrip ();
      //String  menuItemStr = "Remove " + rootName + " From Training Library";
      //cms.Items.Add (menuItemStr, null, RemoveImageFromTrainigLibray);
      //pb.ContextMenuStrip = cms;

      Image thumbNail = image.GetThumbnailImage (w, h, GetThumbnailImageAbort, thumbNailCallBackData);
      pb.Image = thumbNail;

      FlowLayoutPanel pan = new FlowLayoutPanel ();
      pan.Size = new Size (Math.Max (170, w + 10), 260);
      //pan.Height = h + 80;
      //pan.Width  = Math.Max (160, w + 10);

      pan.Controls.Add (pb);
      TextBox tb = new TextBox ();
      tb.Width = pan.Width - 8;
      tb.Text = rootName;
      tb.Font = font;

      pan.Controls.Add (tb);

      tb = new TextBox ();
      tb.Width = pan.Width - 8;
      tb.Text = OSservices.GetRootNameOfDirectory (subDir);
      tb.Font = font;
      pan.Controls.Add (tb);

        tb = new TextBox ();
        tb.Width = pan.Width - 8;
        tb.Text  = probability.ToString ("p2");
        tb.Font  = font;
        pan.Controls.Add (tb);
      
      Button b = new Button ();
      b.Text = "Delete";
      b.Name = rootName;
      b.Enabled = true;
      b.TabStop = false;
      b.Click += new EventHandler (RemoveImageFromTrainigLibray);
      pan.Controls.Add (b);

      pan.BorderStyle = BorderStyle.FixedSingle;

      thumbNails.Add (pan);

      ImageDisplayPanel.Controls.Add (pan);
    }  /* LoadNextImageFromNameList */



    void  RemoveImageFromTrainigLibray (Object sender, EventArgs e)
    {
      if  (sender == null)
        return;
      Button  b = (Button)sender;
      String  rootName = b.Name;

      String  fullName = LocateRootName (rootName);
      if (fullName == null)
        return;

      DialogResult  dr = MessageBox.Show (this, "Delete '" + rootName + "' from Training Library.", "Remove Image", MessageBoxButtons.YesNo);
      if  (dr == DialogResult.Yes)
      {
        b.Enabled = false;
        File.Delete (fullName);
        if  (File.Exists (fullName))
        {
          MessageBox.Show (this, "Failed to delete '" + rootName + "' from Training Library.", "Remove Image");
        }
        else
        {
          MessageBox.Show (this, "'" + rootName + "' Removed form Training Library.", "Remove Image");
        }
      }
    }  /* RemoveImageFromTrainigLibray */



    private  void  ThumbNail_MouseDoubleClick (object sender, MouseEventArgs e)
    {
      if  (e.Button == MouseButtons.Left)
      {
        PictureBox pb = (PictureBox)sender;
        String  imageFileName = pb.Name;
        PicesRaster  pr = new PicesRaster (imageFileName);
        ImageEditor  ie = new ImageEditor (pr);
        ie.ShowDialog (this);
      }
    }



    private  String  lastSelectedImageFileName = "";

    /*
    private  void  PicesThumbNail_MouseDoubleClick (object sender, MouseEventArgs e)
    {
      if  (e.Button == MouseButtons.Left)
      {
        PictureBox pb = (PictureBox)sender;
        String  imageFileName = pb.Name;
        PicesDataBaseImage pi =  dbConn.ImageLoad (imageFileName);
        if  (pi != null)
        {
          PicesRaster  pr = dbConn.ImageFullSizeFind (imageFileName);
          ImageViewer  iv = new ImageViewer (pr, pi, null);
          iv.ShowDialog (this);
        }
      }

    }
     */


    private  String  lastSelectedImage = "";

    private void  MouseClickEvent  (object sender, MouseEventArgs e)
    {
      if  (e.Button == MouseButtons.Right)
      {
        PictureBox pb = (PictureBox)sender;
        lastSelectedImage = pb.Name;
        MenuItem  i1 = new MenuItem ("Copy Image to Clipboard",  CopyImageToClipboard);
        MenuItem  i2 = new MenuItem ("Save Image",               SaveImage);
        MenuItem  i3 = new MenuItem ("View Image",               ViewImage);

        MenuItem[] menuItems = new MenuItem[]{i1, i2, i3};
        ContextMenu  buttonMenu = new ContextMenu (menuItems);
        buttonMenu.Show (pb, e.Location);
      }

      else if  ((e.Button == System.Windows.Forms.MouseButtons.Left)  &&  (e.Clicks == 2))
      {
        PictureBox pb = (PictureBox)sender;
        lastSelectedImage = pb.Name;
        ViewImage (sender, e);
      }
    }

    

    private  void  ViewImage (Object sender, EventArgs e)
    {
      String  imageFileName = lastSelectedImage;
      PicesDataBaseImage pi =  dbConn.ImageLoad (imageFileName);
      if  (pi != null)
      {
        PicesRaster  pr = dbConn.ImageFullSizeFind (imageFileName);
        ImageViewer  iv = new ImageViewer (pr, pi, null);
        iv.ShowDialog (this);
      }
    }


        
    private  void  CopyImageToClipboard (Object sender, EventArgs e)
    {
      String  imageFileName = lastSelectedImage;

      PicesDataBaseImage pi =  dbConn.ImageLoad (imageFileName);
      if  (pi != null)
      {
        PicesRaster  pr = dbConn.ImageFullSizeFind (imageFileName);
        Image  i = pr.BuildBitmap ();
        if  (i != null)
          Clipboard.SetImage (i);
      }
    }  /* CopyImageToClipboard */


    
    private  void  SaveImage (Object sender, EventArgs e)
    {
      String  imageFileName = lastSelectedImage;

      PicesRaster  pr = dbConn.ImageFullSizeFind (imageFileName);
      
      FolderBrowserDialog  fbd = new FolderBrowserDialog ();

      fbd.Description         = "Select directory to save images to";
      fbd.ShowNewFolderButton = true;
      fbd.SelectedPath        = lastSaveImageDir;
      DialogResult  dr = fbd.ShowDialog ();
      if  (dr == DialogResult.OK)
      {
        if  (lastSaveImageDir != fbd.SelectedPath)
          lastSaveImageDir = fbd.SelectedPath;
      
        String  destDir = OSservices.AddSlash (fbd.SelectedPath);
        
        System.IO.DirectoryInfo di = new System.IO.DirectoryInfo (destDir);
        if  (!di.Exists)
        {
          MessageBox.Show (this, "No Such Directory [" + destDir + "]", "Save Images", MessageBoxButtons.OK);
          return;
        }
        
        String  fullFileName = destDir + imageFileName + ".bmp";

        try {pr.Save(fullFileName); }
        catch (Exception  e3)
        {
          MessageBox.Show (this, 
                           "Exception occurred saving image\n\n" + e3.ToString (), 
                           "Save Image",
                           MessageBoxButtons.OK
                          );
        }
      }
    }  /* SaveImage */


    
    private void  UpdateDisplayTimer_Tick (object sender, EventArgs e)
    {
      if  (picesImages != null)
        LoadNextImageFromPicesList ();

      else if  (nameList != null)
        LoadNextImageFromNameList ();

      else
        LoadNextImageFromDir ();
    }



    private void DisplayPicesImages_Load(object sender, EventArgs e)
    {
      lastHeight = Height;
      lastWidth  = Width;
    }



    private void DisplayPicesImages_ResizeEnd(object sender, EventArgs e)
    {
      if  (lastHeight == 0)
        return;

      int  deltaWidth  = Width  - lastWidth;
      int  deltaHeight = Height - lastHeight;

      if  ((deltaHeight == 0)  &&  (deltaWidth == 0))
        return;

      ImageDisplayPanel.Height += deltaHeight;
      ImageDisplayPanel.Width  += deltaWidth;

      ImageDisplayPanel.Controls.Clear ();

      foreach  (FlowLayoutPanel  flp  in  thumbNails)
      {
        ImageDisplayPanel.Controls.Add (flp);
      }

      ImageDisplayPanel.AutoScroll = true;
      ImageDisplayPanel.PerformLayout ();
    } 
  }
}
