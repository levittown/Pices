namespace PicesCommander
{
  partial class ChartSizeDistribution
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
      this.components = new System.ComponentModel.Container();
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
      System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
      System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
      System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
      this.PlotButton = new System.Windows.Forms.Button();
      this.CancelPlotButton = new System.Windows.Forms.Button();
      this.ProfileChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
      this.timer1 = new System.Windows.Forms.Timer(this.components);
      this.Status = new System.Windows.Forms.TextBox();
      this.ClassToPlot = new System.Windows.Forms.TextBox();
      this.SelectClassButton = new System.Windows.Forms.Button();
      this.ClassToPlotLabel = new System.Windows.Forms.Label();
      this.IncludeSubClasses = new System.Windows.Forms.CheckBox();
      this.SelectionCriteriaPanel = new System.Windows.Forms.Panel();
      this.Deployment_ = new System.Windows.Forms.TextBox();
      this.Station_ = new System.Windows.Forms.TextBox();
      this.Cruise_ = new System.Windows.Forms.TextBox();
      this.DeploymentLabel = new System.Windows.Forms.Label();
      this.CruiseLabel = new System.Windows.Forms.Label();
      this.StationLabel = new System.Windows.Forms.Label();
      this.GrowthRateLabel = new System.Windows.Forms.Label();
      this.GrowthRateField = new System.Windows.Forms.NumericUpDown();
      this.MaxSizeLabel = new System.Windows.Forms.Label();
      this.MaxSizeField = new System.Windows.Forms.NumericUpDown();
      this.InitialSizeLabel = new System.Windows.Forms.Label();
      this.InitialSizeField = new System.Windows.Forms.NumericUpDown();
      this.panel1 = new System.Windows.Forms.Panel();
      this.CastLabel = new System.Windows.Forms.Label();
      this.CastField = new System.Windows.Forms.ComboBox();
      this.SizeStatisticField = new System.Windows.Forms.ComboBox();
      this.SizeStatiticLabel = new System.Windows.Forms.Label();
      this.StatusMsgsLabel = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)(this.ProfileChart)).BeginInit();
      this.SelectionCriteriaPanel.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.GrowthRateField)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.MaxSizeField)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.InitialSizeField)).BeginInit();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // PlotButton
      // 
      this.PlotButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.PlotButton.Location = new System.Drawing.Point(12, 587);
      this.PlotButton.Name = "PlotButton";
      this.PlotButton.Size = new System.Drawing.Size(75, 23);
      this.PlotButton.TabIndex = 28;
      this.PlotButton.Text = "Plot";
      this.PlotButton.UseVisualStyleBackColor = true;
      this.PlotButton.Click += new System.EventHandler(this.PlotButton_Click);
      // 
      // CancelPlotButton
      // 
      this.CancelPlotButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CancelPlotButton.Location = new System.Drawing.Point(350, 587);
      this.CancelPlotButton.Name = "CancelPlotButton";
      this.CancelPlotButton.Size = new System.Drawing.Size(88, 23);
      this.CancelPlotButton.TabIndex = 29;
      this.CancelPlotButton.Text = "Cancel";
      this.CancelPlotButton.UseVisualStyleBackColor = true;
      this.CancelPlotButton.Click += new System.EventHandler(this.CancelPlotButton_Click);
      // 
      // ProfileChart
      // 
      this.ProfileChart.BorderlineColor = System.Drawing.Color.Black;
      this.ProfileChart.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      this.ProfileChart.CausesValidation = false;
      chartArea1.AlignmentOrientation = ((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations)((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Vertical | System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Horizontal)));
      chartArea1.AlignWithChartArea = "ChartArea1";
      chartArea1.AxisX.LabelAutoFitMaxFontSize = 12;
      chartArea1.AxisX.LabelStyle.Format = "##0.000";
      chartArea1.AxisX.MajorGrid.Enabled = false;
      chartArea1.AxisX.Title = "Size mm^3";
      chartArea1.AxisX2.LabelStyle.Format = "##0.00";
      chartArea1.AxisX2.MajorGrid.Enabled = false;
      chartArea1.AxisY.IsStartedFromZero = false;
      chartArea1.AxisY.LabelAutoFitMaxFontSize = 12;
      chartArea1.AxisY.LabelStyle.Format = "#0.000";
      chartArea1.AxisY.MajorGrid.Enabled = false;
      chartArea1.AxisY.MajorTickMark.Interval = 0D;
      chartArea1.AxisY.MajorTickMark.IntervalOffset = 0D;
      chartArea1.AxisY.MajorTickMark.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
      chartArea1.AxisY.MajorTickMark.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
      chartArea1.AxisY.Title = "Log10 Denisty";
      chartArea1.AxisY2.MajorGrid.Enabled = false;
      chartArea1.BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea1.Name = "ChartArea1";
      this.ProfileChart.ChartAreas.Add(chartArea1);
      legend1.Alignment = System.Drawing.StringAlignment.Far;
      legend1.DockedToChartArea = "ChartArea1";
      legend1.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Top;
      legend1.Name = "Legend1";
      this.ProfileChart.Legends.Add(legend1);
      this.ProfileChart.Location = new System.Drawing.Point(447, 12);
      this.ProfileChart.Name = "ProfileChart";
      series1.ChartArea = "ChartArea1";
      series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      series1.Legend = "Legend1";
      series1.Name = "Series1";
      this.ProfileChart.Series.Add(series1);
      this.ProfileChart.Size = new System.Drawing.Size(521, 597);
      this.ProfileChart.TabIndex = 30;
      this.ProfileChart.Text = "Abundance/Deployment";
      title1.Name = "Title1";
      this.ProfileChart.Titles.Add(title1);
      this.ProfileChart.MouseClick += new System.Windows.Forms.MouseEventHandler(this.ProfileChart_MouseClick);
      // 
      // timer1
      // 
      this.timer1.Interval = 50;
      this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
      // 
      // Status
      // 
      this.Status.BackColor = System.Drawing.Color.Silver;
      this.Status.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Status.Location = new System.Drawing.Point(12, 289);
      this.Status.Multiline = true;
      this.Status.Name = "Status";
      this.Status.ReadOnly = true;
      this.Status.Size = new System.Drawing.Size(429, 292);
      this.Status.TabIndex = 35;
      this.Status.TabStop = false;
      // 
      // ClassToPlot
      // 
      this.ClassToPlot.Location = new System.Drawing.Point(6, 25);
      this.ClassToPlot.Name = "ClassToPlot";
      this.ClassToPlot.ReadOnly = true;
      this.ClassToPlot.Size = new System.Drawing.Size(343, 20);
      this.ClassToPlot.TabIndex = 52;
      // 
      // SelectClassButton
      // 
      this.SelectClassButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.SelectClassButton.Location = new System.Drawing.Point(355, 24);
      this.SelectClassButton.Name = "SelectClassButton";
      this.SelectClassButton.Size = new System.Drawing.Size(67, 23);
      this.SelectClassButton.TabIndex = 53;
      this.SelectClassButton.Text = "Browse";
      this.SelectClassButton.UseVisualStyleBackColor = true;
      this.SelectClassButton.Click += new System.EventHandler(this.SelectClassButton_Click);
      // 
      // ClassToPlotLabel
      // 
      this.ClassToPlotLabel.AutoSize = true;
      this.ClassToPlotLabel.Location = new System.Drawing.Point(3, 9);
      this.ClassToPlotLabel.Name = "ClassToPlotLabel";
      this.ClassToPlotLabel.Size = new System.Drawing.Size(32, 13);
      this.ClassToPlotLabel.TabIndex = 54;
      this.ClassToPlotLabel.Text = "Class";
      // 
      // IncludeSubClasses
      // 
      this.IncludeSubClasses.AutoSize = true;
      this.IncludeSubClasses.Checked = true;
      this.IncludeSubClasses.CheckState = System.Windows.Forms.CheckState.Checked;
      this.IncludeSubClasses.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IncludeSubClasses.Location = new System.Drawing.Point(6, 79);
      this.IncludeSubClasses.Name = "IncludeSubClasses";
      this.IncludeSubClasses.Size = new System.Drawing.Size(155, 21);
      this.IncludeSubClasses.TabIndex = 55;
      this.IncludeSubClasses.Text = "Include Sub-Classes";
      this.IncludeSubClasses.UseVisualStyleBackColor = true;
      // 
      // SelectionCriteriaPanel
      // 
      this.SelectionCriteriaPanel.BackColor = System.Drawing.Color.Silver;
      this.SelectionCriteriaPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.SelectionCriteriaPanel.Controls.Add(this.Deployment_);
      this.SelectionCriteriaPanel.Controls.Add(this.Station_);
      this.SelectionCriteriaPanel.Controls.Add(this.Cruise_);
      this.SelectionCriteriaPanel.Controls.Add(this.DeploymentLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.CruiseLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.StationLabel);
      this.SelectionCriteriaPanel.Location = new System.Drawing.Point(12, 12);
      this.SelectionCriteriaPanel.Name = "SelectionCriteriaPanel";
      this.SelectionCriteriaPanel.Size = new System.Drawing.Size(429, 72);
      this.SelectionCriteriaPanel.TabIndex = 57;
      // 
      // Deployment_
      // 
      this.Deployment_.Location = new System.Drawing.Point(323, 30);
      this.Deployment_.Name = "Deployment_";
      this.Deployment_.ReadOnly = true;
      this.Deployment_.Size = new System.Drawing.Size(88, 20);
      this.Deployment_.TabIndex = 10;
      // 
      // Station_
      // 
      this.Station_.Location = new System.Drawing.Point(210, 30);
      this.Station_.Name = "Station_";
      this.Station_.ReadOnly = true;
      this.Station_.Size = new System.Drawing.Size(88, 20);
      this.Station_.TabIndex = 9;
      // 
      // Cruise_
      // 
      this.Cruise_.Location = new System.Drawing.Point(3, 30);
      this.Cruise_.Name = "Cruise_";
      this.Cruise_.ReadOnly = true;
      this.Cruise_.Size = new System.Drawing.Size(201, 20);
      this.Cruise_.TabIndex = 8;
      // 
      // DeploymentLabel
      // 
      this.DeploymentLabel.AutoSize = true;
      this.DeploymentLabel.Location = new System.Drawing.Point(320, 14);
      this.DeploymentLabel.Name = "DeploymentLabel";
      this.DeploymentLabel.Size = new System.Drawing.Size(63, 13);
      this.DeploymentLabel.TabIndex = 7;
      this.DeploymentLabel.Text = "Deployment";
      // 
      // CruiseLabel
      // 
      this.CruiseLabel.AutoSize = true;
      this.CruiseLabel.Location = new System.Drawing.Point(3, 14);
      this.CruiseLabel.Name = "CruiseLabel";
      this.CruiseLabel.Size = new System.Drawing.Size(36, 13);
      this.CruiseLabel.TabIndex = 3;
      this.CruiseLabel.Text = "Cruise";
      // 
      // StationLabel
      // 
      this.StationLabel.AutoSize = true;
      this.StationLabel.Location = new System.Drawing.Point(207, 14);
      this.StationLabel.Name = "StationLabel";
      this.StationLabel.Size = new System.Drawing.Size(40, 13);
      this.StationLabel.TabIndex = 5;
      this.StationLabel.Text = "Station";
      // 
      // GrowthRateLabel
      // 
      this.GrowthRateLabel.AutoSize = true;
      this.GrowthRateLabel.Location = new System.Drawing.Point(180, 122);
      this.GrowthRateLabel.Name = "GrowthRateLabel";
      this.GrowthRateLabel.Size = new System.Drawing.Size(64, 13);
      this.GrowthRateLabel.TabIndex = 64;
      this.GrowthRateLabel.Text = "GrowthRate";
      // 
      // GrowthRateField
      // 
      this.GrowthRateField.DecimalPlaces = 4;
      this.GrowthRateField.Increment = new decimal(new int[] {
            5,
            0,
            0,
            131072});
      this.GrowthRateField.Location = new System.Drawing.Point(181, 138);
      this.GrowthRateField.Name = "GrowthRateField";
      this.GrowthRateField.Size = new System.Drawing.Size(74, 20);
      this.GrowthRateField.TabIndex = 63;
      this.GrowthRateField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.GrowthRateField.Value = new decimal(new int[] {
            11,
            0,
            0,
            65536});
      // 
      // MaxSizeLabel
      // 
      this.MaxSizeLabel.AutoSize = true;
      this.MaxSizeLabel.Location = new System.Drawing.Point(94, 122);
      this.MaxSizeLabel.Name = "MaxSizeLabel";
      this.MaxSizeLabel.Size = new System.Drawing.Size(55, 13);
      this.MaxSizeLabel.TabIndex = 62;
      this.MaxSizeLabel.Text = "Max Sizse";
      // 
      // MaxSizeField
      // 
      this.MaxSizeField.Location = new System.Drawing.Point(95, 138);
      this.MaxSizeField.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
      this.MaxSizeField.Name = "MaxSizeField";
      this.MaxSizeField.Size = new System.Drawing.Size(63, 20);
      this.MaxSizeField.TabIndex = 61;
      this.MaxSizeField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.MaxSizeField.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
      // 
      // InitialSizeLabel
      // 
      this.InitialSizeLabel.AutoSize = true;
      this.InitialSizeLabel.Location = new System.Drawing.Point(4, 122);
      this.InitialSizeLabel.Name = "InitialSizeLabel";
      this.InitialSizeLabel.Size = new System.Drawing.Size(54, 13);
      this.InitialSizeLabel.TabIndex = 60;
      this.InitialSizeLabel.Text = "Initial Size";
      // 
      // InitialSizeField
      // 
      this.InitialSizeField.DecimalPlaces = 3;
      this.InitialSizeField.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
      this.InitialSizeField.Location = new System.Drawing.Point(5, 138);
      this.InitialSizeField.Name = "InitialSizeField";
      this.InitialSizeField.Size = new System.Drawing.Size(70, 20);
      this.InitialSizeField.TabIndex = 60;
      this.InitialSizeField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.InitialSizeField.Value = new decimal(new int[] {
            4,
            0,
            0,
            131072});
      // 
      // panel1
      // 
      this.panel1.BackColor = System.Drawing.Color.Silver;
      this.panel1.Controls.Add(this.CastLabel);
      this.panel1.Controls.Add(this.CastField);
      this.panel1.Controls.Add(this.SizeStatisticField);
      this.panel1.Controls.Add(this.SizeStatiticLabel);
      this.panel1.Controls.Add(this.ClassToPlotLabel);
      this.panel1.Controls.Add(this.GrowthRateLabel);
      this.panel1.Controls.Add(this.GrowthRateField);
      this.panel1.Controls.Add(this.MaxSizeLabel);
      this.panel1.Controls.Add(this.ClassToPlot);
      this.panel1.Controls.Add(this.MaxSizeField);
      this.panel1.Controls.Add(this.SelectClassButton);
      this.panel1.Controls.Add(this.InitialSizeLabel);
      this.panel1.Controls.Add(this.IncludeSubClasses);
      this.panel1.Controls.Add(this.InitialSizeField);
      this.panel1.Location = new System.Drawing.Point(12, 94);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(429, 167);
      this.panel1.TabIndex = 65;
      // 
      // CastLabel
      // 
      this.CastLabel.AutoSize = true;
      this.CastLabel.Location = new System.Drawing.Point(324, 64);
      this.CastLabel.Name = "CastLabel";
      this.CastLabel.Size = new System.Drawing.Size(28, 13);
      this.CastLabel.TabIndex = 68;
      this.CastLabel.Text = "Cast";
      // 
      // CastField
      // 
      this.CastField.FormattingEnabled = true;
      this.CastField.Items.AddRange(new object[] {
            "Down",
            "Up",
            "Both"});
      this.CastField.Location = new System.Drawing.Point(324, 79);
      this.CastField.Name = "CastField";
      this.CastField.Size = new System.Drawing.Size(88, 21);
      this.CastField.TabIndex = 57;
      this.CastField.Text = "Both";
      // 
      // SizeStatisticField
      // 
      this.SizeStatisticField.FormattingEnabled = true;
      this.SizeStatisticField.Items.AddRange(new object[] {
            "Pixel Area",
            "Diameter",
            "Volume (SBv)",
            "Volume (EBv)"});
      this.SizeStatisticField.Location = new System.Drawing.Point(279, 138);
      this.SizeStatisticField.Name = "SizeStatisticField";
      this.SizeStatisticField.Size = new System.Drawing.Size(133, 21);
      this.SizeStatisticField.TabIndex = 67;
      this.SizeStatisticField.Text = "Pixel Area";
      // 
      // SizeStatiticLabel
      // 
      this.SizeStatiticLabel.AutoSize = true;
      this.SizeStatiticLabel.Location = new System.Drawing.Point(276, 122);
      this.SizeStatiticLabel.Name = "SizeStatiticLabel";
      this.SizeStatiticLabel.Size = new System.Drawing.Size(67, 13);
      this.SizeStatiticLabel.TabIndex = 66;
      this.SizeStatiticLabel.Text = "Size Statistic";
      // 
      // StatusMsgsLabel
      // 
      this.StatusMsgsLabel.AutoSize = true;
      this.StatusMsgsLabel.Location = new System.Drawing.Point(13, 273);
      this.StatusMsgsLabel.Name = "StatusMsgsLabel";
      this.StatusMsgsLabel.Size = new System.Drawing.Size(37, 13);
      this.StatusMsgsLabel.TabIndex = 65;
      this.StatusMsgsLabel.Text = "Status";
      // 
      // ChartSizeDistribution
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(981, 620);
      this.Controls.Add(this.StatusMsgsLabel);
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.SelectionCriteriaPanel);
      this.Controls.Add(this.Status);
      this.Controls.Add(this.ProfileChart);
      this.Controls.Add(this.CancelPlotButton);
      this.Controls.Add(this.PlotButton);
      this.MinimumSize = new System.Drawing.Size(997, 620);
      this.Name = "ChartSizeDistribution";
      this.Text = "Chart Size Distribution";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ChartSizeDistribution_FormClosing);
      this.Load += new System.EventHandler(this.ChartAbundanceByDeployment_Load);
      this.SizeChanged += new System.EventHandler(this.ChartAbundanceByDeployment_SizeChanged);
      this.Resize += new System.EventHandler(this.ChartAbundanceByDeployment_Resize);
      ((System.ComponentModel.ISupportInitialize)(this.ProfileChart)).EndInit();
      this.SelectionCriteriaPanel.ResumeLayout(false);
      this.SelectionCriteriaPanel.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.GrowthRateField)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.MaxSizeField)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.InitialSizeField)).EndInit();
      this.panel1.ResumeLayout(false);
      this.panel1.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button PlotButton;
    private System.Windows.Forms.Button CancelPlotButton;
    private System.Windows.Forms.DataVisualization.Charting.Chart ProfileChart;
    private System.Windows.Forms.Timer timer1;
    private System.Windows.Forms.TextBox Status;
    private System.Windows.Forms.TextBox ClassToPlot;
    private System.Windows.Forms.Button SelectClassButton;
    private System.Windows.Forms.Label ClassToPlotLabel;
    private System.Windows.Forms.CheckBox IncludeSubClasses;
    private System.Windows.Forms.Panel SelectionCriteriaPanel;
    private System.Windows.Forms.Label DeploymentLabel;
    private System.Windows.Forms.Label CruiseLabel;
    private System.Windows.Forms.Label StationLabel;
    private System.Windows.Forms.TextBox Deployment_;
    private System.Windows.Forms.TextBox Station_;
    private System.Windows.Forms.TextBox Cruise_;
    private System.Windows.Forms.Label GrowthRateLabel;
    private System.Windows.Forms.NumericUpDown GrowthRateField;
    private System.Windows.Forms.Label MaxSizeLabel;
    private System.Windows.Forms.NumericUpDown MaxSizeField;
    private System.Windows.Forms.Label InitialSizeLabel;
    private System.Windows.Forms.NumericUpDown InitialSizeField;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Label StatusMsgsLabel;
    private System.Windows.Forms.ComboBox SizeStatisticField;
    private System.Windows.Forms.Label SizeStatiticLabel;
    private System.Windows.Forms.Label CastLabel;
    private System.Windows.Forms.ComboBox CastField;


  }
}