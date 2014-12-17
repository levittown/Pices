namespace PicesCommander
{
  partial class ChartDistribution
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.components = new System.ComponentModel.Container ();
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea ();
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea ();
      System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend ();
      System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend ();
      System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series ();
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint1 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1500, 0);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint2 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (17000, 5);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint3 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (5000, 15);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint4 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1800, 20);
      System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series ();
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint5 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1.265, 0.123);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint6 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1.776, 0.976);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint7 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1.01, 3.672);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint8 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (0.256, 10);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint9 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (-0.765, 15);
      System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series ();
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint10 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (7, 0.66);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint11 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (1.8, 3.54);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint12 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (3.75, 3.8);
      System.Windows.Forms.DataVisualization.Charting.DataPoint dataPoint13 = new System.Windows.Forms.DataVisualization.Charting.DataPoint (7.56, 12);
      System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title ();
      System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title ();
      this.ClassesToPlot = new System.Windows.Forms.DataGridView ();
      this.Class = new System.Windows.Forms.DataGridViewTextBoxColumn ();
      this.BrowseClasses = new System.Windows.Forms.DataGridViewButtonColumn ();
      this.Remove = new System.Windows.Forms.DataGridViewButtonColumn ();
      this.ColorToUse = new System.Windows.Forms.DataGridViewButtonColumn ();
      this.IncludeSubClasses = new System.Windows.Forms.CheckBox ();
      this.AddClassButton = new System.Windows.Forms.Button ();
      this.WeightByVolume = new System.Windows.Forms.CheckBox ();
      this.Inst1ToPlotLabel = new System.Windows.Forms.Label ();
      this.SelectionCriteriaPanel = new System.Windows.Forms.Panel ();
      this.ProbRange = new System.Windows.Forms.TextBox ();
      this.ProbRangeLabel = new System.Windows.Forms.Label ();
      this.DepthRange = new System.Windows.Forms.TextBox ();
      this.DepthRangeLabel = new System.Windows.Forms.Label ();
      this.SizeRange = new System.Windows.Forms.TextBox ();
      this.SizePrametersLabel = new System.Windows.Forms.Label ();
      this.Deployment = new System.Windows.Forms.TextBox ();
      this.DeploymentLabel = new System.Windows.Forms.Label ();
      this.CruiseLabel = new System.Windows.Forms.Label ();
      this.Station = new System.Windows.Forms.TextBox ();
      this.Cruise = new System.Windows.Forms.TextBox ();
      this.StationLabel = new System.Windows.Forms.Label ();
      this.Inst1ToPlot = new System.Windows.Forms.ComboBox ();
      this.Inst2ToPlot = new System.Windows.Forms.ComboBox ();
      this.Inst2ToPlotLabel = new System.Windows.Forms.Label ();
      this.PlotButton = new System.Windows.Forms.Button ();
      this.CancelPlotButton = new System.Windows.Forms.Button ();
      this.DepthProfileChart = new System.Windows.Forms.DataVisualization.Charting.Chart ();
      this.DepthIncr = new System.Windows.Forms.NumericUpDown ();
      this.timer1 = new System.Windows.Forms.Timer (this.components);
      this.DepthIncrLabel = new System.Windows.Forms.Label ();
      this.Inst1ColorButton = new System.Windows.Forms.Button ();
      this.Inst2ColorButton = new System.Windows.Forms.Button ();
      this.Status = new System.Windows.Forms.TextBox ();
      ((System.ComponentModel.ISupportInitialize)(this.ClassesToPlot)).BeginInit ();
      this.SelectionCriteriaPanel.SuspendLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.DepthProfileChart)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.DepthIncr)).BeginInit ();
      this.SuspendLayout ();
      // 
      // ClassesToPlot
      // 
      this.ClassesToPlot.AllowUserToAddRows = false;
      this.ClassesToPlot.AllowUserToDeleteRows = false;
      this.ClassesToPlot.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.ClassesToPlot.Columns.AddRange (new System.Windows.Forms.DataGridViewColumn[] {
            this.Class,
            this.BrowseClasses,
            this.Remove,
            this.ColorToUse});
      this.ClassesToPlot.Location = new System.Drawing.Point (12, 136);
      this.ClassesToPlot.Name = "ClassesToPlot";
      this.ClassesToPlot.RowHeadersVisible = false;
      this.ClassesToPlot.Size = new System.Drawing.Size (469, 242);
      this.ClassesToPlot.TabIndex = 6;
      this.ClassesToPlot.CellFormatting += new System.Windows.Forms.DataGridViewCellFormattingEventHandler (this.ClassesToPlot_CellFormatting);
      this.ClassesToPlot.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler (this.ClassesToPlot_RowsAdded);
      this.ClassesToPlot.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler (this.ClassesToPlot_CellClick);
      // 
      // Class
      // 
      this.Class.HeaderText = "Class";
      this.Class.Name = "Class";
      this.Class.Width = 280;
      // 
      // BrowseClasses
      // 
      this.BrowseClasses.HeaderText = "Browse";
      this.BrowseClasses.Name = "BrowseClasses";
      this.BrowseClasses.Text = "Browse";
      this.BrowseClasses.UseColumnTextForButtonValue = true;
      this.BrowseClasses.Width = 60;
      // 
      // Remove
      // 
      this.Remove.HeaderText = "Remove";
      this.Remove.Name = "Remove";
      this.Remove.Text = "Remove";
      this.Remove.UseColumnTextForButtonValue = true;
      this.Remove.Width = 60;
      // 
      // ColorToUse
      // 
      this.ColorToUse.HeaderText = "Color";
      this.ColorToUse.Name = "ColorToUse";
      this.ColorToUse.Text = "Color";
      this.ColorToUse.Width = 60;
      // 
      // IncludeSubClasses
      // 
      this.IncludeSubClasses.AutoSize = true;
      this.IncludeSubClasses.Checked = true;
      this.IncludeSubClasses.CheckState = System.Windows.Forms.CheckState.Checked;
      this.IncludeSubClasses.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IncludeSubClasses.Location = new System.Drawing.Point (93, 398);
      this.IncludeSubClasses.Name = "IncludeSubClasses";
      this.IncludeSubClasses.Size = new System.Drawing.Size (155, 21);
      this.IncludeSubClasses.TabIndex = 7;
      this.IncludeSubClasses.Text = "Include Sub-Classes";
      this.IncludeSubClasses.UseVisualStyleBackColor = true;
      // 
      // AddClassButton
      // 
      this.AddClassButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AddClassButton.Location = new System.Drawing.Point (12, 394);
      this.AddClassButton.Name = "AddClassButton";
      this.AddClassButton.Size = new System.Drawing.Size (75, 27);
      this.AddClassButton.TabIndex = 8;
      this.AddClassButton.Text = "Add Class";
      this.AddClassButton.UseVisualStyleBackColor = true;
      this.AddClassButton.Click += new System.EventHandler (this.AddClassButton_Click);
      // 
      // WeightByVolume
      // 
      this.WeightByVolume.AutoSize = true;
      this.WeightByVolume.Checked = true;
      this.WeightByVolume.CheckState = System.Windows.Forms.CheckState.Checked;
      this.WeightByVolume.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.WeightByVolume.Location = new System.Drawing.Point (259, 398);
      this.WeightByVolume.Name = "WeightByVolume";
      this.WeightByVolume.Size = new System.Drawing.Size (141, 21);
      this.WeightByVolume.TabIndex = 9;
      this.WeightByVolume.Text = "Weight by Volume";
      this.WeightByVolume.UseVisualStyleBackColor = true;
      // 
      // Inst1ToPlotLabel
      // 
      this.Inst1ToPlotLabel.AutoSize = true;
      this.Inst1ToPlotLabel.Location = new System.Drawing.Point (15, 445);
      this.Inst1ToPlotLabel.Name = "Inst1ToPlotLabel";
      this.Inst1ToPlotLabel.Size = new System.Drawing.Size (57, 13);
      this.Inst1ToPlotLabel.TabIndex = 11;
      this.Inst1ToPlotLabel.Text = "Inst to Plot";
      // 
      // SelectionCriteriaPanel
      // 
      this.SelectionCriteriaPanel.BackColor = System.Drawing.Color.FromArgb (((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
      this.SelectionCriteriaPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.SelectionCriteriaPanel.Controls.Add (this.ProbRange);
      this.SelectionCriteriaPanel.Controls.Add (this.ProbRangeLabel);
      this.SelectionCriteriaPanel.Controls.Add (this.DepthRange);
      this.SelectionCriteriaPanel.Controls.Add (this.DepthRangeLabel);
      this.SelectionCriteriaPanel.Controls.Add (this.SizeRange);
      this.SelectionCriteriaPanel.Controls.Add (this.SizePrametersLabel);
      this.SelectionCriteriaPanel.Controls.Add (this.Deployment);
      this.SelectionCriteriaPanel.Controls.Add (this.DeploymentLabel);
      this.SelectionCriteriaPanel.Controls.Add (this.CruiseLabel);
      this.SelectionCriteriaPanel.Controls.Add (this.Station);
      this.SelectionCriteriaPanel.Controls.Add (this.Cruise);
      this.SelectionCriteriaPanel.Controls.Add (this.StationLabel);
      this.SelectionCriteriaPanel.Location = new System.Drawing.Point (12, 12);
      this.SelectionCriteriaPanel.Name = "SelectionCriteriaPanel";
      this.SelectionCriteriaPanel.Size = new System.Drawing.Size (470, 104);
      this.SelectionCriteriaPanel.TabIndex = 24;
      // 
      // ProbRange
      // 
      this.ProbRange.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ProbRange.Location = new System.Drawing.Point (325, 69);
      this.ProbRange.Name = "ProbRange";
      this.ProbRange.ReadOnly = true;
      this.ProbRange.Size = new System.Drawing.Size (130, 23);
      this.ProbRange.TabIndex = 30;
      this.ProbRange.TabStop = false;
      // 
      // ProbRangeLabel
      // 
      this.ProbRangeLabel.AutoSize = true;
      this.ProbRangeLabel.Location = new System.Drawing.Point (254, 74);
      this.ProbRangeLabel.Name = "ProbRangeLabel";
      this.ProbRangeLabel.Size = new System.Drawing.Size (64, 13);
      this.ProbRangeLabel.TabIndex = 29;
      this.ProbRangeLabel.Text = "Prob Range";
      // 
      // DepthRange
      // 
      this.DepthRange.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.DepthRange.Location = new System.Drawing.Point (325, 39);
      this.DepthRange.Name = "DepthRange";
      this.DepthRange.ReadOnly = true;
      this.DepthRange.Size = new System.Drawing.Size (130, 23);
      this.DepthRange.TabIndex = 28;
      this.DepthRange.TabStop = false;
      // 
      // DepthRangeLabel
      // 
      this.DepthRangeLabel.AutoSize = true;
      this.DepthRangeLabel.Location = new System.Drawing.Point (254, 44);
      this.DepthRangeLabel.Name = "DepthRangeLabel";
      this.DepthRangeLabel.Size = new System.Drawing.Size (71, 13);
      this.DepthRangeLabel.TabIndex = 27;
      this.DepthRangeLabel.Text = "Depth Range";
      // 
      // SizeRange
      // 
      this.SizeRange.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SizeRange.Location = new System.Drawing.Point (325, 9);
      this.SizeRange.Name = "SizeRange";
      this.SizeRange.ReadOnly = true;
      this.SizeRange.Size = new System.Drawing.Size (130, 23);
      this.SizeRange.TabIndex = 26;
      this.SizeRange.TabStop = false;
      // 
      // SizePrametersLabel
      // 
      this.SizePrametersLabel.AutoSize = true;
      this.SizePrametersLabel.Location = new System.Drawing.Point (254, 14);
      this.SizePrametersLabel.Name = "SizePrametersLabel";
      this.SizePrametersLabel.Size = new System.Drawing.Size (62, 13);
      this.SizePrametersLabel.TabIndex = 25;
      this.SizePrametersLabel.Text = "Size Range";
      // 
      // Deployment
      // 
      this.Deployment.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Deployment.Location = new System.Drawing.Point (85, 69);
      this.Deployment.Name = "Deployment";
      this.Deployment.ReadOnly = true;
      this.Deployment.Size = new System.Drawing.Size (150, 23);
      this.Deployment.TabIndex = 8;
      this.Deployment.TabStop = false;
      // 
      // DeploymentLabel
      // 
      this.DeploymentLabel.AutoSize = true;
      this.DeploymentLabel.Location = new System.Drawing.Point (14, 74);
      this.DeploymentLabel.Name = "DeploymentLabel";
      this.DeploymentLabel.Size = new System.Drawing.Size (63, 13);
      this.DeploymentLabel.TabIndex = 7;
      this.DeploymentLabel.Text = "Deployment";
      // 
      // CruiseLabel
      // 
      this.CruiseLabel.AutoSize = true;
      this.CruiseLabel.Location = new System.Drawing.Point (14, 14);
      this.CruiseLabel.Name = "CruiseLabel";
      this.CruiseLabel.Size = new System.Drawing.Size (36, 13);
      this.CruiseLabel.TabIndex = 3;
      this.CruiseLabel.Text = "Cruise";
      // 
      // Station
      // 
      this.Station.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Station.Location = new System.Drawing.Point (85, 39);
      this.Station.Name = "Station";
      this.Station.ReadOnly = true;
      this.Station.Size = new System.Drawing.Size (150, 23);
      this.Station.TabIndex = 6;
      this.Station.TabStop = false;
      // 
      // Cruise
      // 
      this.Cruise.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Cruise.Location = new System.Drawing.Point (85, 9);
      this.Cruise.Name = "Cruise";
      this.Cruise.ReadOnly = true;
      this.Cruise.Size = new System.Drawing.Size (150, 23);
      this.Cruise.TabIndex = 4;
      this.Cruise.TabStop = false;
      // 
      // StationLabel
      // 
      this.StationLabel.AutoSize = true;
      this.StationLabel.Location = new System.Drawing.Point (14, 44);
      this.StationLabel.Name = "StationLabel";
      this.StationLabel.Size = new System.Drawing.Size (40, 13);
      this.StationLabel.TabIndex = 5;
      this.StationLabel.Text = "Station";
      // 
      // Inst1ToPlot
      // 
      this.Inst1ToPlot.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Inst1ToPlot.FormattingEnabled = true;
      this.Inst1ToPlot.Location = new System.Drawing.Point (12, 459);
      this.Inst1ToPlot.Name = "Inst1ToPlot";
      this.Inst1ToPlot.Size = new System.Drawing.Size (160, 24);
      this.Inst1ToPlot.TabIndex = 25;
      this.Inst1ToPlot.Enter += new System.EventHandler (this.Inst1ToPlot_Enter);
      // 
      // Inst2ToPlot
      // 
      this.Inst2ToPlot.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Inst2ToPlot.FormattingEnabled = true;
      this.Inst2ToPlot.Location = new System.Drawing.Point (269, 459);
      this.Inst2ToPlot.Name = "Inst2ToPlot";
      this.Inst2ToPlot.Size = new System.Drawing.Size (160, 24);
      this.Inst2ToPlot.TabIndex = 26;
      this.Inst2ToPlot.Enter += new System.EventHandler (this.Inst2ToPlot_Enter);
      // 
      // Inst2ToPlotLabel
      // 
      this.Inst2ToPlotLabel.AutoSize = true;
      this.Inst2ToPlotLabel.Location = new System.Drawing.Point (266, 445);
      this.Inst2ToPlotLabel.Name = "Inst2ToPlotLabel";
      this.Inst2ToPlotLabel.Size = new System.Drawing.Size (57, 13);
      this.Inst2ToPlotLabel.TabIndex = 27;
      this.Inst2ToPlotLabel.Text = "Inst to Plot";
      // 
      // PlotButton
      // 
      this.PlotButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PlotButton.Location = new System.Drawing.Point (12, 561);
      this.PlotButton.Name = "PlotButton";
      this.PlotButton.Size = new System.Drawing.Size (75, 23);
      this.PlotButton.TabIndex = 28;
      this.PlotButton.Text = "Plot";
      this.PlotButton.UseVisualStyleBackColor = true;
      this.PlotButton.Click += new System.EventHandler (this.PlotButton_Click);
      // 
      // CancelPlotButton
      // 
      this.CancelPlotButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CancelPlotButton.Location = new System.Drawing.Point (407, 561);
      this.CancelPlotButton.Name = "CancelPlotButton";
      this.CancelPlotButton.Size = new System.Drawing.Size (75, 23);
      this.CancelPlotButton.TabIndex = 29;
      this.CancelPlotButton.Text = "Cancel";
      this.CancelPlotButton.UseVisualStyleBackColor = true;
      this.CancelPlotButton.Click += new System.EventHandler (this.CancelPlotButton_Click);
      // 
      // DepthProfileChart
      // 
      this.DepthProfileChart.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      this.DepthProfileChart.CausesValidation = false;
      chartArea1.AlignmentOrientation = System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Horizontal;
      chartArea1.AlignmentStyle = ((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles)((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles.PlotPosition | System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles.AxesView)));
      chartArea1.AlignWithChartArea = "ChartArea2";
      chartArea1.AxisX.MajorGrid.Enabled = false;
      chartArea1.AxisX.Minimum = 0;
      chartArea1.AxisX.Title = "Count/m^3";
      chartArea1.AxisX2.MajorGrid.Enabled = false;
      chartArea1.AxisY.IsReversed = true;
      chartArea1.AxisY.MajorGrid.Enabled = false;
      chartArea1.AxisY.Maximum = 25;
      chartArea1.AxisY.Minimum = 0;
      chartArea1.AxisY.Title = "Depth in Meters";
      chartArea1.AxisY2.MajorGrid.Enabled = false;
      chartArea1.BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea1.Name = "ChartArea1";
      chartArea1.Position.Auto = false;
      chartArea1.Position.Height = 80F;
      chartArea1.Position.Width = 45F;
      chartArea1.Position.Y = 10F;
      chartArea2.AlignmentOrientation = System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Horizontal;
      chartArea2.AlignmentStyle = System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles.AxesView;
      chartArea2.AlignWithChartArea = "ChartArea1";
      chartArea2.AxisX.MajorGrid.Enabled = false;
      chartArea2.AxisX.Minimum = 0;
      chartArea2.AxisX.Title = "ml/L";
      chartArea2.AxisX2.MajorGrid.Enabled = false;
      chartArea2.AxisX2.Title = "Celcius";
      chartArea2.AxisY.IsReversed = true;
      chartArea2.AxisY.MajorGrid.Enabled = false;
      chartArea2.AxisY.Maximum = 25;
      chartArea2.AxisY.Minimum = 0;
      chartArea2.AxisY2.MajorGrid.Enabled = false;
      chartArea2.BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea2.Name = "ChartArea2";
      chartArea2.Position.Auto = false;
      chartArea2.Position.Height = 80F;
      chartArea2.Position.Width = 45F;
      chartArea2.Position.X = 48F;
      chartArea2.Position.Y = 10F;
      this.DepthProfileChart.ChartAreas.Add (chartArea1);
      this.DepthProfileChart.ChartAreas.Add (chartArea2);
      legend1.DockedToChartArea = "ChartArea1";
      legend1.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
      legend1.Name = "Legend1";
      legend2.DockedToChartArea = "ChartArea2";
      legend2.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
      legend2.Name = "Legend2";
      this.DepthProfileChart.Legends.Add (legend1);
      this.DepthProfileChart.Legends.Add (legend2);
      this.DepthProfileChart.Location = new System.Drawing.Point (498, 12);
      this.DepthProfileChart.Name = "DepthProfileChart";
      series1.BorderWidth = 2;
      series1.ChartArea = "ChartArea1";
      series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series1.Legend = "Legend1";
      series1.Name = "Copepods";
      series1.Points.Add (dataPoint1);
      series1.Points.Add (dataPoint2);
      series1.Points.Add (dataPoint3);
      series1.Points.Add (dataPoint4);
      series2.ChartArea = "ChartArea2";
      series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series2.Legend = "Legend2";
      series2.Name = "Oxygen";
      series2.Points.Add (dataPoint5);
      series2.Points.Add (dataPoint6);
      series2.Points.Add (dataPoint7);
      series2.Points.Add (dataPoint8);
      series2.Points.Add (dataPoint9);
      series3.ChartArea = "ChartArea2";
      series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series3.Legend = "Legend2";
      series3.Name = "Florenses";
      series3.Points.Add (dataPoint10);
      series3.Points.Add (dataPoint11);
      series3.Points.Add (dataPoint12);
      series3.Points.Add (dataPoint13);
      series3.XAxisType = System.Windows.Forms.DataVisualization.Charting.AxisType.Secondary;
      this.DepthProfileChart.Series.Add (series1);
      this.DepthProfileChart.Series.Add (series2);
      this.DepthProfileChart.Series.Add (series3);
      this.DepthProfileChart.Size = new System.Drawing.Size (463, 572);
      this.DepthProfileChart.TabIndex = 30;
      this.DepthProfileChart.Text = "Verticle Profile";
      title1.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      title1.Name = "Title1";
      title1.Text = "Verticle Profile";
      title2.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      title2.Name = "Title2";
      title2.Text = "Cruise: jksdjsk  Station:XXX  Deployment: Alpa";
      this.DepthProfileChart.Titles.Add (title1);
      this.DepthProfileChart.Titles.Add (title2);
      // 
      // DepthIncr
      // 
      this.DepthIncr.DecimalPlaces = 1;
      this.DepthIncr.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.DepthIncr.Increment = new decimal (new int[] {
            5,
            0,
            0,
            65536});
      this.DepthIncr.Location = new System.Drawing.Point (406, 398);
      this.DepthIncr.Minimum = new decimal (new int[] {
            5,
            0,
            0,
            65536});
      this.DepthIncr.Name = "DepthIncr";
      this.DepthIncr.Size = new System.Drawing.Size (76, 23);
      this.DepthIncr.TabIndex = 31;
      this.DepthIncr.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      // 
      // timer1
      // 
      this.timer1.Interval = 50;
      this.timer1.Tick += new System.EventHandler (this.timer1_Tick);
      // 
      // DepthIncrLabel
      // 
      this.DepthIncrLabel.AutoSize = true;
      this.DepthIncrLabel.Location = new System.Drawing.Point (403, 382);
      this.DepthIncrLabel.Name = "DepthIncrLabel";
      this.DepthIncrLabel.Size = new System.Drawing.Size (86, 13);
      this.DepthIncrLabel.TabIndex = 32;
      this.DepthIncrLabel.Text = "Depth Increment";
      // 
      // Inst1ColorButton
      // 
      this.Inst1ColorButton.Location = new System.Drawing.Point (176, 459);
      this.Inst1ColorButton.Name = "Inst1ColorButton";
      this.Inst1ColorButton.Size = new System.Drawing.Size (49, 23);
      this.Inst1ColorButton.TabIndex = 33;
      this.Inst1ColorButton.Text = "Color";
      this.Inst1ColorButton.UseVisualStyleBackColor = true;
      this.Inst1ColorButton.Click += new System.EventHandler (this.Inst1ColorButton_Click);
      // 
      // Inst2ColorButton
      // 
      this.Inst2ColorButton.Location = new System.Drawing.Point (433, 459);
      this.Inst2ColorButton.Name = "Inst2ColorButton";
      this.Inst2ColorButton.Size = new System.Drawing.Size (49, 23);
      this.Inst2ColorButton.TabIndex = 34;
      this.Inst2ColorButton.Text = "Color";
      this.Inst2ColorButton.UseVisualStyleBackColor = true;
      this.Inst2ColorButton.Click += new System.EventHandler (this.Inst2ColorButton_Click);
      // 
      // Status
      // 
      this.Status.Font = new System.Drawing.Font ("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Status.Location = new System.Drawing.Point (12, 514);
      this.Status.Name = "Status";
      this.Status.ReadOnly = true;
      this.Status.Size = new System.Drawing.Size (470, 23);
      this.Status.TabIndex = 35;
      this.Status.TabStop = false;
      // 
      // ChartDistribution
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (967, 597);
      this.Controls.Add (this.Status);
      this.Controls.Add (this.Inst2ColorButton);
      this.Controls.Add (this.Inst1ColorButton);
      this.Controls.Add (this.DepthIncrLabel);
      this.Controls.Add (this.DepthIncr);
      this.Controls.Add (this.DepthProfileChart);
      this.Controls.Add (this.CancelPlotButton);
      this.Controls.Add (this.PlotButton);
      this.Controls.Add (this.Inst2ToPlotLabel);
      this.Controls.Add (this.Inst2ToPlot);
      this.Controls.Add (this.Inst1ToPlot);
      this.Controls.Add (this.SelectionCriteriaPanel);
      this.Controls.Add (this.Inst1ToPlotLabel);
      this.Controls.Add (this.WeightByVolume);
      this.Controls.Add (this.AddClassButton);
      this.Controls.Add (this.IncludeSubClasses);
      this.Controls.Add (this.ClassesToPlot);
      this.MinimumSize = new System.Drawing.Size (983, 635);
      this.Name = "ChartDistribution";
      this.Text = "Depth Profile Chart";
      this.Load += new System.EventHandler (this.ChartDistribution_Load);
      this.SizeChanged += new System.EventHandler (this.ChartDistribution_SizeChanged);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.ChartDistribution_FormClosing);
      this.Resize += new System.EventHandler (this.ChartDistribution_Resize);
      ((System.ComponentModel.ISupportInitialize)(this.ClassesToPlot)).EndInit ();
      this.SelectionCriteriaPanel.ResumeLayout (false);
      this.SelectionCriteriaPanel.PerformLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.DepthProfileChart)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.DepthIncr)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.DataGridView ClassesToPlot;
    private System.Windows.Forms.CheckBox IncludeSubClasses;
    private System.Windows.Forms.Button AddClassButton;
    private System.Windows.Forms.CheckBox WeightByVolume;
    private System.Windows.Forms.Label Inst1ToPlotLabel;
    private System.Windows.Forms.Panel SelectionCriteriaPanel;
    private System.Windows.Forms.TextBox DepthRange;
    private System.Windows.Forms.Label DepthRangeLabel;
    private System.Windows.Forms.TextBox SizeRange;
    private System.Windows.Forms.Label SizePrametersLabel;
    private System.Windows.Forms.TextBox Deployment;
    private System.Windows.Forms.Label DeploymentLabel;
    private System.Windows.Forms.Label CruiseLabel;
    private System.Windows.Forms.TextBox Station;
    private System.Windows.Forms.TextBox Cruise;
    private System.Windows.Forms.Label StationLabel;
    private System.Windows.Forms.TextBox ProbRange;
    private System.Windows.Forms.Label ProbRangeLabel;
    private System.Windows.Forms.ComboBox Inst1ToPlot;
    private System.Windows.Forms.ComboBox Inst2ToPlot;
    private System.Windows.Forms.Label Inst2ToPlotLabel;
    private System.Windows.Forms.Button PlotButton;
    private System.Windows.Forms.Button CancelPlotButton;
    private System.Windows.Forms.DataVisualization.Charting.Chart DepthProfileChart;
    private System.Windows.Forms.NumericUpDown DepthIncr;
    private System.Windows.Forms.Timer timer1;
    private System.Windows.Forms.Label DepthIncrLabel;
    private System.Windows.Forms.DataGridViewTextBoxColumn Class;
    private System.Windows.Forms.DataGridViewButtonColumn BrowseClasses;
    private System.Windows.Forms.DataGridViewButtonColumn Remove;
    private System.Windows.Forms.DataGridViewButtonColumn ColorToUse;
    private System.Windows.Forms.Button Inst1ColorButton;
    private System.Windows.Forms.Button Inst2ColorButton;
    private System.Windows.Forms.TextBox Status;


  }
}