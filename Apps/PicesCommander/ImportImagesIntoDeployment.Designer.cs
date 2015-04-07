namespace PicesCommander
{
  partial class  ImportImagesIntoDeployment
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ImportImagesIntoDeployment));
      this.TitleLabel = new System.Windows.Forms.Label();
      this.SourceDirectoryLabel = new System.Windows.Forms.Label();
      this.SourceDirectory = new System.Windows.Forms.TextBox();
      this.BrowseSourceDir = new System.Windows.Forms.Button();
      this.Start = new System.Windows.Forms.Button();
      this.Cancel = new System.Windows.Forms.Button();
      this.RunLog = new System.Windows.Forms.TextBox();
      this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
      this.ImportTimer = new System.Windows.Forms.Timer(this.components);
      this.ApplicationDescription = new System.Windows.Forms.TextBox();
      this.CruiseNamelabel = new System.Windows.Forms.Label();
      this.CruiseField = new System.Windows.Forms.ComboBox();
      this.StationField = new System.Windows.Forms.ComboBox();
      this.StationLabel = new System.Windows.Forms.Label();
      this.DeploymentField = new System.Windows.Forms.ComboBox();
      this.Deploymentlabel = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
      this.SuspendLayout();
      // 
      // TitleLabel
      // 
      this.TitleLabel.AutoSize = true;
      this.TitleLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TitleLabel.Location = new System.Drawing.Point(194, 9);
      this.TitleLabel.Name = "TitleLabel";
      this.TitleLabel.Size = new System.Drawing.Size(269, 24);
      this.TitleLabel.TabIndex = 0;
      this.TitleLabel.Text = "Import Images into Deployment";
      // 
      // SourceDirectoryLabel
      // 
      this.SourceDirectoryLabel.AutoSize = true;
      this.SourceDirectoryLabel.Location = new System.Drawing.Point(13, 120);
      this.SourceDirectoryLabel.Name = "SourceDirectoryLabel";
      this.SourceDirectoryLabel.Size = new System.Drawing.Size(86, 13);
      this.SourceDirectoryLabel.TabIndex = 1;
      this.SourceDirectoryLabel.Text = "Source Directory";
      // 
      // SourceDirectory
      // 
      this.SourceDirectory.Location = new System.Drawing.Point(102, 117);
      this.SourceDirectory.Name = "SourceDirectory";
      this.SourceDirectory.Size = new System.Drawing.Size(433, 20);
      this.SourceDirectory.TabIndex = 50;
      this.SourceDirectory.Leave += new System.EventHandler(this.SourceDirectory_Leave);
      // 
      // BrowseSourceDir
      // 
      this.BrowseSourceDir.Location = new System.Drawing.Point(540, 115);
      this.BrowseSourceDir.Name = "BrowseSourceDir";
      this.BrowseSourceDir.Size = new System.Drawing.Size(75, 23);
      this.BrowseSourceDir.TabIndex = 60;
      this.BrowseSourceDir.Text = "Browse";
      this.BrowseSourceDir.UseVisualStyleBackColor = true;
      this.BrowseSourceDir.Click += new System.EventHandler(this.BrowseSourceDir_Click);
      // 
      // Start
      // 
      this.Start.Location = new System.Drawing.Point(19, 541);
      this.Start.Name = "Start";
      this.Start.Size = new System.Drawing.Size(75, 23);
      this.Start.TabIndex = 6;
      this.Start.Text = "Start";
      this.Start.UseVisualStyleBackColor = true;
      this.Start.Click += new System.EventHandler(this.Start_Click);
      // 
      // Cancel
      // 
      this.Cancel.Location = new System.Drawing.Point(543, 541);
      this.Cancel.Name = "Cancel";
      this.Cancel.Size = new System.Drawing.Size(75, 23);
      this.Cancel.TabIndex = 7;
      this.Cancel.Text = "Cancel";
      this.Cancel.UseVisualStyleBackColor = true;
      this.Cancel.Click += new System.EventHandler(this.Cancel_Click);
      // 
      // RunLog
      // 
      this.RunLog.Location = new System.Drawing.Point(19, 283);
      this.RunLog.Multiline = true;
      this.RunLog.Name = "RunLog";
      this.RunLog.ReadOnly = true;
      this.RunLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.RunLog.Size = new System.Drawing.Size(599, 242);
      this.RunLog.TabIndex = 8;
      this.RunLog.TabStop = false;
      // 
      // errorProvider1
      // 
      this.errorProvider1.ContainerControl = this;
      // 
      // ImportTimer
      // 
      this.ImportTimer.Tick += new System.EventHandler(this.ImportTimer_Tick);
      // 
      // ApplicationDescription
      // 
      this.ApplicationDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ApplicationDescription.Location = new System.Drawing.Point(19, 158);
      this.ApplicationDescription.Multiline = true;
      this.ApplicationDescription.Name = "ApplicationDescription";
      this.ApplicationDescription.ReadOnly = true;
      this.ApplicationDescription.Size = new System.Drawing.Size(596, 119);
      this.ApplicationDescription.TabIndex = 9;
      this.ApplicationDescription.Text = resources.GetString("ApplicationDescription.Text");
      // 
      // CruiseNamelabel
      // 
      this.CruiseNamelabel.AutoSize = true;
      this.CruiseNamelabel.Location = new System.Drawing.Point(22, 56);
      this.CruiseNamelabel.Name = "CruiseNamelabel";
      this.CruiseNamelabel.Size = new System.Drawing.Size(36, 13);
      this.CruiseNamelabel.TabIndex = 10;
      this.CruiseNamelabel.Text = "Cruise";
      // 
      // CruiseField
      // 
      this.CruiseField.FormattingEnabled = true;
      this.CruiseField.Location = new System.Drawing.Point(19, 73);
      this.CruiseField.Name = "CruiseField";
      this.CruiseField.Size = new System.Drawing.Size(143, 21);
      this.CruiseField.TabIndex = 10;
      this.CruiseField.SelectedIndexChanged += new System.EventHandler(this.CruiseField_SelectedIndexChanged);
      this.CruiseField.Format += new System.Windows.Forms.ListControlConvertEventHandler(this.CruiseField_Format);
      // 
      // StationField
      // 
      this.StationField.FormattingEnabled = true;
      this.StationField.Location = new System.Drawing.Point(178, 73);
      this.StationField.Name = "StationField";
      this.StationField.Size = new System.Drawing.Size(102, 21);
      this.StationField.TabIndex = 10;
      this.StationField.SelectedIndexChanged += new System.EventHandler(this.StationField_SelectedIndexChanged);
      this.StationField.Format += new System.Windows.Forms.ListControlConvertEventHandler(this.StationField_Format);
      // 
      // StationLabel
      // 
      this.StationLabel.AutoSize = true;
      this.StationLabel.Location = new System.Drawing.Point(182, 56);
      this.StationLabel.Name = "StationLabel";
      this.StationLabel.Size = new System.Drawing.Size(40, 13);
      this.StationLabel.TabIndex = 12;
      this.StationLabel.Text = "Station";
      // 
      // DeploymentField
      // 
      this.DeploymentField.FormattingEnabled = true;
      this.DeploymentField.Location = new System.Drawing.Point(302, 73);
      this.DeploymentField.Name = "DeploymentField";
      this.DeploymentField.Size = new System.Drawing.Size(100, 21);
      this.DeploymentField.TabIndex = 30;
      this.DeploymentField.SelectedIndexChanged += new System.EventHandler(this.DeploymentField_SelectedIndexChanged);
      this.DeploymentField.Format += new System.Windows.Forms.ListControlConvertEventHandler(this.DeploymentField_Format);
      // 
      // Deploymentlabel
      // 
      this.Deploymentlabel.AutoSize = true;
      this.Deploymentlabel.Location = new System.Drawing.Point(306, 56);
      this.Deploymentlabel.Name = "Deploymentlabel";
      this.Deploymentlabel.Size = new System.Drawing.Size(63, 13);
      this.Deploymentlabel.TabIndex = 14;
      this.Deploymentlabel.Text = "Deployment";
      // 
      // ImportImagesIntoDeployment
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(630, 572);
      this.Controls.Add(this.DeploymentField);
      this.Controls.Add(this.Deploymentlabel);
      this.Controls.Add(this.StationField);
      this.Controls.Add(this.StationLabel);
      this.Controls.Add(this.CruiseField);
      this.Controls.Add(this.CruiseNamelabel);
      this.Controls.Add(this.ApplicationDescription);
      this.Controls.Add(this.RunLog);
      this.Controls.Add(this.Cancel);
      this.Controls.Add(this.Start);
      this.Controls.Add(this.BrowseSourceDir);
      this.Controls.Add(this.SourceDirectory);
      this.Controls.Add(this.SourceDirectoryLabel);
      this.Controls.Add(this.TitleLabel);
      this.MaximumSize = new System.Drawing.Size(646, 610);
      this.MinimumSize = new System.Drawing.Size(646, 610);
      this.Name = "ImportImagesIntoDeployment";
      this.Text = "ImportGroupAssignments";
      this.Load += new System.EventHandler(this.ImportImagesIntoDeployment_Load);
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label TitleLabel;
    private System.Windows.Forms.Label SourceDirectoryLabel;
    private System.Windows.Forms.TextBox SourceDirectory;
    private System.Windows.Forms.Button BrowseSourceDir;
    private System.Windows.Forms.Button Start;
    private System.Windows.Forms.Button Cancel;
    private System.Windows.Forms.TextBox RunLog;
    private System.Windows.Forms.ErrorProvider errorProvider1;
    private System.Windows.Forms.Timer ImportTimer;
    private System.Windows.Forms.TextBox ApplicationDescription;
    private System.Windows.Forms.ComboBox DeploymentField;
    private System.Windows.Forms.Label Deploymentlabel;
    private System.Windows.Forms.ComboBox StationField;
    private System.Windows.Forms.Label StationLabel;
    private System.Windows.Forms.ComboBox CruiseField;
    private System.Windows.Forms.Label CruiseNamelabel;
  }
}