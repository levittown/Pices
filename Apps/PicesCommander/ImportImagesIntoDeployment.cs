using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;

using  PicesInterface;

namespace PicesCommander
{
  public partial class ImportImagesIntoDeployment : Form
  {
    private  PicesDataBase           mainWinConn = null;
    private  PicesRunLog             runLog = null;
    private  PicesMsgQueue           msgQueue = null;

    private  String                  sourceDirectory = null;

    // Importing Thread Related Variables.
    private  bool                    cancelImporting    = false;
    private  bool                    importingStarted   = false;  /** Will be set to 'true' after the start button is pressed and all validations are passed. */
    private  bool                    importingRunning   = false;
    private  bool                    importingCompleted = false;
    private  Thread                  importingThread    = null;

    // 
    private bool                     validationErrorFound = false;

    private int                      totalImagesUpdated = 0;

    public  bool                     CancelImporting     {get {return cancelImporting;}   set  {cancelImporting = value;}}
    public  bool                     ImportingRunning    {get {return importingRunning;}}


    private  bool                    importImages = false;   /**< Indicates that we are importing images into the database; not just validating them. */

    private  PicesSipperFile         sipperFile     = null;

    private  PicesDataBaseLogEntry   extractionLogEntryId = null;

    private  PicesClass              unknownClass  = null;

    private  String                  cruiseName = "";
    private  String                  stationName = "";
    private  String                  deploymentNum = "";

    private  PicesSipperCruise       cruise     = null;
    private  PicesSipperStation      station    = null;
    private  PicesSipperDeployment   deployment = null;


    public  ImportImagesIntoDeployment (String  _cruiseName,
                                        String  _stationName,
                                        String  _deploymentNum
                                       )
    {
      msgQueue = new PicesMsgQueue ("ImportImagesIntoDeployment");
      runLog  = new PicesRunLog (msgQueue);

      cruiseName    = _cruiseName;
      stationName   = _stationName;
      deploymentNum = _deploymentNum;

      mainWinConn  = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);

      unknownClass = GetClassFromName (mainWinConn, "UnKnown");

      InitializeComponent();

      SourceDirectory.Text = PicesSipperVariables.PicesHomeDir ();
      SourceDirectory.Text = "D:\\Users\\kkramer\\PlanktonCompetition\\trunk\\Data\\";
    }


    private void ImportImagesIntoDeployment_Load(object sender, EventArgs e)
    {
      LoadCruises ();
      if  (cruise != null)
      {
        LoadStations ();
        if  (station != null)
          LoadDeployments ();
      }
    }


    private  void  LoadCruises ()
    {
      cruise = null;
      PicesSipperCruiseList cruises = mainWinConn.SipperCruiseLoadAllCruises ();
      CruiseField.Items.Clear ();
      foreach  (PicesSipperCruise c in cruises)
      {
        CruiseField.Items.Add (c);
        if  (c.CruiseName.Equals (cruiseName, StringComparison.InvariantCultureIgnoreCase))
          cruise = c;
      }
      if  (cruise == null)
        cruiseName = "";
      else
        CruiseField.SelectedItem = cruise;
    }


    private  void  LoadStations ()
    {
      station = null;
      PicesSipperStationList  stations = mainWinConn.SipperStationLoad (cruise.CruiseName);
      if  (stations == null)
        return;
      StationField.Items.Clear ();
      foreach  (PicesSipperStation s in  stations)
      {
        StationField.Items.Add (s);
        if  (s.StationName.Equals (stationName, StringComparison.InvariantCultureIgnoreCase))
          station = s;
      }
      if  (station == null)
        stationName = "";
      else
        StationField.SelectedItem = station;
    }


    private  void  LoadDeployments ()
    {
      deployment = null;
      PicesSipperDeploymentList  deployments = mainWinConn.SipperDeploymentLoad (cruiseName, stationName);
      if  (deployments == null)
        return;
      DeploymentField.Items.Clear ();
      deployment = null;
      foreach  (PicesSipperDeployment d in  deployments)
      {
        DeploymentField.Items.Add (d);
        if  (d.DeploymentNum.Equals (deploymentNum, StringComparison.InvariantCultureIgnoreCase))
          deployment = d;
      }
      if  (deployment == null)
        deploymentNum = "";
      else
        DeploymentField.SelectedItem = deployment;
    }





    private  void  RunLogMsgQueueFlush ()
    {
      if  (msgQueue != null)
      {
        String  m = msgQueue.GetNextMsg ();
        while  (m != null)
        {
          RunLog.AppendText (m);
          m = msgQueue.GetNextMsg ();
        }
      }
    }  /* RunLogMsgQueueFlush */



    private  void  ValidateCruiseName ()
    {
      errorProvider1.SetError (CruiseField, "");
      if  (CruiseField.SelectedItem == null)
      {
        errorProvider1.SetError (CruiseField, "No cruise is selected.");
        StationField.Items.Clear ();
        validationErrorFound = true;
      }
      else
      {
        cruise = (PicesSipperCruise)CruiseField.SelectedItem;
        cruiseName = cruise.CruiseName;
      }
    }


    private  void  ValidateStation ()
    {
      errorProvider1.SetError (StationField, "");
      if  (StationField.SelectedItem == null)
      {
        errorProvider1.SetError (StationField, "No Station Selected.");
        validationErrorFound = true;
      }
      else
      {
        station = (PicesSipperStation)StationField.SelectedItem;
        stationName = station.StationName;
      }
    }



    private  void  ValidateDeployment ()
    {
      errorProvider1.SetError (DeploymentField, "");
      if  (DeploymentField.SelectedItem == null)
      {
        errorProvider1.SetError (DeploymentField, "No Deployment Selected.");
        validationErrorFound = true;
      }
      else
      {
        deployment = (PicesSipperDeployment)DeploymentField.SelectedItem;
        deploymentNum = deployment.DeploymentNum;
      }
    }




    private  void  ValidateSourceDirectory ()
    {
      errorProvider1.SetError (SourceDirectory, "");

      String  parentDir = OSservices.GetParentDirectoryOfDirPath (SourceDirectory.Text);
      DirectoryInfo di = new DirectoryInfo (SourceDirectory.Text);
      if  (!di.Exists)
      {
        validationErrorFound = true;
        errorProvider1.SetError (SourceDirectory, "No such directory[" + SourceDirectory.Text + "].");
      }
      else
      {
        sourceDirectory = SourceDirectory.Text;
      }
    }  /* ValidateSourceDirectory */

    

    private void ValidateAll ()
    {
      validationErrorFound = false;
      ValidateCruiseName ();
      ValidateStation    ();
      ValidateDeployment ();
      ValidateSourceDirectory ();
    }  /* ValidateAll */



    private  void  StartTheImportingProcedure ()
    {
      importingStarted = true;
      cancelImporting = false;
      importingThread = new Thread (new ThreadStart (ImportImagesIntoDeploymentProcess));
      importingThread.Start ();

      ImportTimer.Enabled      = true;
      SourceDirectory.Enabled  = false;
    }  /* StartTheHarvestingProcedure */


    
    private DateTime ExecutableDateTimeStamp ()
    {
      DateTime  compilationDateTime;
      System.IO.FileInfo fi = new System.IO.FileInfo(Application.ExecutablePath.Trim());
      try
      {
        compilationDateTime =  fi.LastWriteTime;
      }
      catch (Exception)
      {
        compilationDateTime = DateTime.Now;
      }
      finally
      {
        fi = null;
      }

      return  compilationDateTime;
    }  /* ExecutableDateTimeStamp */




    private  String  ToCmdLine ()
    {
      String  cmdLine = "";

      if  (!String.IsNullOrEmpty (sourceDirectory))
        cmdLine += " -SourceDir " + sourceDirectory;

      cmdLine += "  -Cruise " + cruiseName;
      cmdLine += "  -Station " + stationName;
      if  (!String.IsNullOrEmpty (deploymentNum))
        cmdLine += "  -Deployment " + deploymentNum;

      if  (mainWinConn == null)
        cmdLine += " -NoDatabase ";
      else
        cmdLine += (" -DataBase " + mainWinConn.Server.HostName);

      cmdLine = cmdLine.TrimStart (' ');

      return  cmdLine;
    }  /* ToCmdLine */



    private  void  ImportImagesIntoDeploymentProcess ()
    {
      importingRunning = true;
      importingCompleted = false;

      PicesDataBase.ThreadInit ();

      PicesRunLog  runLog = new PicesRunLog ();
      PicesDataBase  threadConn = PicesDataBase.GetGlobalDatabaseManagerNewInstance (runLog);
     
      extractionLogEntryId = threadConn.LogEntriesProcessStart ("II", 
                                                                "ImportImagesIntoDeployment",  
                                                                ExecutableDateTimeStamp (),
                                                                ToCmdLine (),
                                                                ""
                                                               );
      totalImagesUpdated = 0;

      ImportImagesFromDirectoryStructure (threadConn, sourceDirectory);
      if  (cancelImporting)
      {
        runLog.Writeln ("\n" + "Importing Assignments has been Canceled." + "\n\n");
      }
      else
      {
        runLog.Writeln ("\n\n" + "Total Images[" + totalImagesUpdated.ToString ("###,##0") + "\n\n");
      }

      threadConn.Close ();
      threadConn = null;
      GC.Collect ();

      PicesDataBase.ThreadEnd ();

      importingRunning = false;
      importingCompleted = true;
    }  /* ImportImagesIntoDeploymentProcess */



    private  PicesClass  GetClassFromName (PicesDataBase  threadConn,
                                           String         _name
                                          )
    {
      PicesClass pc = threadConn.MLClassLoad (_name);
      if  (pc == null)
      {
        pc = new PicesClass (_name, "");
        threadConn.MLClassInsert (pc);
      }
      return  pc;
    }


    private  void  ImportImage (PicesDataBase  threadConn,
                                String         sipperFileRootName,
                                String         fileName,
                                ref bool       successful
                               )
    {
      successful = false;
      String  rootName = OSservices.GetRootName (fileName);
      String  picesRootName = sipperFileRootName + "_" + rootName;

      PicesDataBaseImage  dbi = threadConn.ImageLoad (picesRootName);
      if  (dbi != null)
        return;

      PicesRaster r = new PicesRaster (fileName);
      r = r.Padded (2);

      PicesFeatureVector  fv = new PicesFeatureVector (r, picesRootName, null, runLog);
      fv.ImageFileName = picesRootName;

      int   imageId    = 0;


      uint  centroidRow = (uint)(0.5f + fv.CentroidRow);
      uint  centroidCol = (uint)(0.5f + fv.CentroidCol);

      if  ((centroidRow < 0)  ||  (centroidRow > r.Height))
        centroidRow = (uint)(r.Height / 2);

      if  ((centroidCol < 0)  ||  (centroidCol > r.Width))
        centroidCol = (uint)(r.Width / 2);

      threadConn.ImageInsert (r,
                              picesRootName,
                              sipperFileRootName, 
                              0, 0, 0, 
                              (uint)r.Height,  (uint)r.Width,
                              (uint)fv.OrigSize, 
                              3,
                              extractionLogEntryId.LogEntryId, extractionLogEntryId.LogEntryId,
                              centroidRow, centroidCol,
                              unknownClass, 1.0f, null, 0.0f, null, 
                              0.0f,  // depth,
                              0.0f,  // Image Size
                              null,
                              ref imageId,
                              ref successful
                             );
      if  (successful)
      {
        threadConn.FeatureDataInsertRow (picesRootName, fv);
      }
      else
      {
        runLog.Writeln ("RootName[" + rootName + "] Failed: " + threadConn.LastErrorDesc () + "\n");
      }
    }  /* ImportImage */



    private  String[]  FilterListForImageFiles (String[]  fileNames)
    {
      List<String>  results = new List<string> ();

      foreach  (String s in  fileNames)
      {
        String ext = OSservices.GetFileExtension (s);
        if  (ext.Equals ("bmp", StringComparison.InvariantCultureIgnoreCase)  ||
             ext.Equals ("jpg", StringComparison.InvariantCultureIgnoreCase)
            )
          results.Add (s);
      }
      if  (results.Count < 1)
        return null;
      else
        return  results.ToArray ();
    }  /* FilterListFirImageFiles */




    private  void  ImportImagesFromDirectoryStructure (PicesDataBase  threadConn,
                                                       String         dirName
                                                      )
    {
      runLog.Writeln ("Dir[" + dirName + "]" + "\n");

      String[]  fileNames = null;
      try  {fileNames = Directory.GetFiles (dirName);}  
      catch  (Exception e) 
      {
        runLog.Writeln ("\n" + "Error retrieving file info for Directory[" + dirName + "]" + "\n");
        runLog.Writeln ("Exception[" + e.ToString () + "]" + "\n\n");
        fileNames = null;
      }

      if  (fileNames != null)
        fileNames = FilterListForImageFiles (fileNames);

      if  (fileNames != null)
      {
        String  sipperFileName = cruiseName + stationName + deploymentNum + "-" + OSservices.GetRootName (dirName);

        sipperFile = new PicesSipperFile (sipperFileName);
        threadConn.SipperFileInsert (sipperFile);

        int numThisDir       = 0;
        int numFailedThisDir = 0;
        foreach (String fn in fileNames)
        {
          String  ext = OSservices.GetFileExtension (fn).ToLower ();
          if  ((ext == "bmp")  ||  (ext == "jpg"))
          {
            numThisDir++;
            String  fullName = OSservices.AddSlash (dirName) + fn;
            bool  successful = false;
            ImportImage (threadConn, sipperFileName, fullName, ref successful);
            if  (!successful)
              ++numFailedThisDir;

            if  ((numThisDir  % 100) == 0)
              runLog.Writeln ("Dir[" + dirName + "]  Files Updated[" + numThisDir.ToString ("###,##0") + "]" + "\n");
          }
          if  (cancelImporting)
            break;
        }

        runLog.Writeln ("Dir[" + dirName + "]  Files Updated[" + numThisDir.ToString ("###,##0") + "]" + "\n");
        totalImagesUpdated += numThisDir;
      }

      if  (!cancelImporting)
      {
        String[]  directories = null;
        try {directories = Directory.GetDirectories (dirName);}
        catch(Exception) {directories = null;}

        if  (directories != null)
        {
          foreach  (String  subDir in directories)
          {
            if  ((subDir == ".")  ||  (subDir == ".."))
              continue;
            ImportImagesFromDirectoryStructure (threadConn, subDir);
            if  (cancelImporting)
              break;
          }
        }
      }
    }  /* ImportImagesFromDirectoryStructure */



    private  void  CancelImportingProcess ()
    {
      cancelImporting = true;
      int  loopCount = 0;

      while  (importingRunning  && (loopCount < 200)  &&   importingThread.IsAlive)
      {
        Thread.Sleep (100);
        RunLogMsgQueueFlush ();
        loopCount++;
      }

      if  ((importingThread != null)  &&  (importingThread.IsAlive))
        importingThread.Abort ();
    }  /* CancelImportingProcess */




    private void BrowseSourceDir_Click(object sender, EventArgs e)
    {
      FolderBrowserDialog  fbd = new FolderBrowserDialog ();

      if  (String.IsNullOrEmpty (SourceDirectory.Text))
        SourceDirectory.Text = PicesSipperVariables.PicesHomeDir ();

      if  (!Directory.Exists (SourceDirectory.Text))
        SourceDirectory.Text = PicesSipperVariables.PicesHomeDir ();

      fbd.SelectedPath = SourceDirectory.Text;

      DialogResult dr = fbd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        SourceDirectory.Text = fbd.SelectedPath;
        ValidateSourceDirectory ();
      }
    }



    private void SourceDirectory_Leave(object sender, EventArgs e)
    {
      ValidateSourceDirectory ();
    }


   
    private void Start_Click(object sender, EventArgs e)
    {
      if  (importingStarted)
      {
        // This should not have been able to have happened.
        Start.Enabled = false;
        return;
      }

      ValidateAll ();
      if  (validationErrorFound)
        return;

      DialogResult  dr = MessageBox.Show (this, "Start Importing Images Assignments?", "Import Group Assignments", MessageBoxButtons.YesNo);
      if  (dr == DialogResult.No)
        return;

      Start.Enabled = false;

      StartTheImportingProcedure ();
    }



    private void Cancel_Click (object sender, EventArgs e)
    {
      Cancel.Enabled = false;

      if  (!importingRunning)
      {
        this.Close ();
      }

      DialogResult dr = MessageBox.Show (this, "Cancel Importing ?", "Importing", MessageBoxButtons.YesNo);
      if  (dr == DialogResult.Yes)
      {
        CancelImportingProcess ();
        this.Close ();
      }

      Cancel.Enabled = true;
    }


    private void ImportTimer_Tick(object sender, EventArgs e)
    {
      RunLogMsgQueueFlush ();

      if  (!importingRunning)
      {
        ImportTimer.Enabled = false;

        if ((importingCompleted)  &&  (!cancelImporting))
          MessageBox.Show (this, "Importing of Validated Class Assignments completed.");

        this.Close ();
      }
    }

    private void CruiseField_Format (object sender, ListControlConvertEventArgs e)
    {
      if  (e.ListItem == null)
        return;
      PicesSipperCruise  c = (PicesSipperCruise)e.ListItem;
      String  m = c.CruiseName + " - " + c.DateStart.ToString ("yyyy-MM-dd");
      e.Value = m;
    }

    private void StationField_Format(object sender, ListControlConvertEventArgs e)
    {
      if  (e.ListItem == null)
        return;
      PicesSipperStation  s = (PicesSipperStation)e.ListItem;
      String  m = s.StationName;
      e.Value = m;
    }


    private void DeploymentField_Format(object sender, ListControlConvertEventArgs e)
    {
      if  (e.ListItem == null)
        return;
      PicesSipperDeployment  d = (PicesSipperDeployment)e.ListItem;
      String  m = d.DeploymentNum;
      if  (!String.IsNullOrEmpty (m))
        m += " - ";
      m += d.DateTimeStart.ToString ("yyyy-MM-dd");
      e.Value = m;
    }


    private void  CruiseField_SelectedIndexChanged (object sender, EventArgs e)
    {
      if  ((PicesSipperCruise)CruiseField.SelectedItem != cruise)
      {
        ValidateCruiseName ();
        LoadStations ();
      }
    }


    private void StationField_SelectedIndexChanged(object sender, EventArgs e)
    {
      if  ((PicesSipperStation)StationField.SelectedItem != station)
      {
        validationErrorFound = false;
        ValidateStation ();
        if  (!validationErrorFound)
          LoadDeployments ();
        }
    }

    private void DeploymentField_SelectedIndexChanged(object sender, EventArgs e)
    {
      if  ((PicesSipperDeployment)DeploymentField.SelectedItem != deployment)
      {
        validationErrorFound = false;
        ValidateDeployment ();
      }
    }
  }
}
