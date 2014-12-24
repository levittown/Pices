using System;
using System.Collections.Generic;
using System.Text;

using  PicesInterface;


namespace PicesCommander
{

//#define  BucketCount  50
//#define  BucketSize   100





  public  class  SizeDistribution 
  {
    public  class  ClassTotals  
    {
      int      bucketCount = 0;
      int      bucketSize  = 0;
      int      count       = 0;
      String   name        = null;
      String   nameUpper   = null;
      int[]    sizeBuckets = null;


      public  ClassTotals (String _name,
                           int    _bucketCount,
                           int    _bucketSize
                          )
      {
        bucketCount  = _bucketCount;
        bucketSize   = _bucketSize;
        count        = 0;
        name         = _name;
        nameUpper    = _name.ToUpper ();
        sizeBuckets  = new int[bucketCount];
        for  (int x = 0; x < bucketCount; x++)
           sizeBuckets[x] = 0;
        count = 0;
      }

        
      public  void  CleaUpMemory ()
      {
        sizeBuckets = null;
      } 

      public  String  Name       {get  {return name;}}

      public  String  NameUpper  {get  {return  nameUpper;}}

      public  void  Increment (int  size)
      {
        int  bucket = (int)((float)size / (float)bucketSize);
        if  (bucket >= bucketCount)
          bucket = bucketCount - 1;

        if  (bucket < 0)
          bucket = 0;

        sizeBuckets[bucket]++;
        count++;
      }



      public  void  AddIn (ClassTotals  classTotals)
      {
        int  idx;

        count = count + classTotals.count;

        for  (idx = 0; idx < bucketCount; idx++)
          sizeBuckets[idx] = sizeBuckets[idx] + classTotals.sizeBuckets[idx];
      }  /* AddIn */



      public  int   BucketCount (int idx)
      {
        if  ((idx < 0)  &&  (idx >= bucketCount))
          return 0;
        return  sizeBuckets[idx];
      }


      public  void  PrintFormatedLine (System.IO.StreamWriter  o)
      {
        String  s = name.PadRight (20)   ;
        o.Write (s);

        s = count.ToString ().PadLeft (9);
        o.Write (s);

        int  bucket;

        for (bucket = 0;  bucket < bucketCount;  bucket++)
        {
          s = sizeBuckets [bucket].ToString ().PadLeft (8);
          o.Write (s);
        }
        o.WriteLine ();
      }  /* PrintFormatedLine */



      public  void  PrintCSVLine (System.IO.StreamWriter  o)
      {
        o.Write ("\"" + name + "\"," + count.ToString ());

        int  bucket;

        for (bucket = 0;  bucket <  bucketCount; bucket++)
        {
          o.Write ("," + sizeBuckets[bucket].ToString ());
        }
        o.WriteLine ();
      }  /* PrintCSVLine */




      public  void  PrintTabDelLine (System.IO.StreamWriter  o)
      {
        o.Write ("\"" + name + "\"" + "\t" + count.ToString ());

        int  bucket;

        for (bucket = 0;  bucket <  bucketCount; bucket++)
          o.Write ("\t" + sizeBuckets[bucket].ToString ());

        o.WriteLine ();
      }
    };  /* PrintTabDelLine */




    class  ClassTotalsList:  List<ClassTotals>
    {
      public  ClassTotalsList ()
      {
      }
      

      public  void  CleanUpMemory ()
      {
        foreach  (ClassTotals ct in this)
        {
          ct.CleaUpMemory ();
        }
        Clear ();
      }
      
   
      public  ClassTotals  LookUp (String  _name)
      {
        _name = _name.ToUpper ();

        int  idx;
        ClassTotals  classTotals = null;
        ClassTotals  temp        = null;

        for  (idx = 0; (idx < Count)  &&  (classTotals == null);  idx++)
        {
          temp = (this)[idx];
          if  (temp.NameUpper == _name)
            classTotals = temp;
        }

        return  classTotals;
      }  /* LookUp */



      private class  ClassTotalsComparator:  IComparer<ClassTotals>
      {
        int  IComparer<ClassTotals>.Compare (ClassTotals x, ClassTotals y)  
        {
          return x.Name.CompareTo (y.Name);
        }
      }  /* ClassTotalsComparator */



      public  void  SortByName ()
      {
        Sort (new ClassTotalsComparator ());
      }

    };  /* ClassTotalsList */

    
    
    
    int              bucketCount = 0;
    int              bucketSize  = 0;
    ClassTotalsList  totals      = null;


    public  SizeDistribution (int  _bucketCount,
                              int  _bucketSize
                             )
    {
      bucketCount = _bucketCount;
      bucketSize  = _bucketSize;

      totals = new ClassTotalsList ();
      if  (bucketCount < 1)
        throw new Exception ("SizeDistribution     *** Invalid Bucket Size[" + bucketSize.ToString () + "] ***");
    }

  

    /// <summary>
    /// Returns an instance of 'ClassTotals' that contains a summary of the classes that are decendent of 'ancestor'.
    /// The newly instantiated instance will point to the same instances of 'ClassTotals'  that this instance points to.
    /// As a result you should only call this method after you are done accumulating data.
    /// </summary>
    /// <param name="ancestor"></param>
    /// <returns></returns>
    public  int  SumUpFamilyOfClassesForBucket (PicesClass  ancestor,
                                                int         bucketIdx
                                               )
    {
      int  bucketTotal = 0;
      ClassTotals  ct = totals.LookUp (ancestor.Name);
      if  (ct != null)
        bucketTotal += ct.BucketCount (bucketIdx);

      if  (ancestor.Children != null)
      {
        foreach (PicesClass pc in ancestor.Children)
          bucketTotal += SumUpFamilyOfClassesForBucket (pc, bucketIdx);
      }
      return  bucketTotal;
    }  /* SumUpFamilyOfClassesForBucket */



    

    /// <summary>
    /// Returns an instance of 'ClassTotals' that contains a summary of the classes that are decendent of 'ancestor'.
    /// The newly instantiated instance will point to the same instances of 'ClassTotals'  that this instance points to.
    /// As a result you should only call this method after you are done accumulating data.
    /// </summary>
    /// <param name="ancestor"></param>
    /// <returns></returns>
    public  ClassTotals  SumUpFamilyOfClasses (PicesClass  ancestor)
    {
      ClassTotals  familySummary = new ClassTotals (ancestor.Name, bucketCount, bucketSize);
      AddFamilyOfClassesToSizeClassTotals (familySummary, ancestor);
      return   familySummary;
    }  /* ExtractFamilyOfClasses */




    private  void  AddFamilyOfClassesToSizeClassTotals (ClassTotals   summary,
                                                        PicesClass    ancestor
                                                       )
    {
      ClassTotals  ct = totals.LookUp (ancestor.Name);
      if  (ct != null)
        summary.AddIn (ct);
      
      if  (ancestor.Children != null)
      {
        foreach  (PicesClass  pc in ancestor.Children)
          AddFamilyOfClassesToSizeClassTotals (summary, pc);
      }

      return;
    }  /* AddFamilyOfClassesToSizeClassTotals */




    /// <summary>
    /// Will add in the contents of another SizeDistribution instance into this instance.
    /// </summary>
    /// <param name="x">The other 'SizeDistribution' instance to be acumulated to this instance.</param>
    public  void  Add (SizeDistribution  x)
    {
      if  ((x.bucketCount != this.bucketCount)  ||  (x.bucketSize != this.bucketSize))
      {
        throw new Exception ("'SizeDistribution.Add   ***ERROR***   Dimensions are not the same" + "\n" +
                             "Left.bucketCount[" + bucketCount.ToString () + "]  Right[" + x.bucketCount.ToString () + "]" + "\n" +
                             "Left.bucketSize [" +this.bucketSize.ToString () + "]  Right.bucketSize[" + x.bucketSize.ToString () + "]"
                             );
      }

      foreach (ClassTotals  ct in x.totals)
      {
        ClassTotals  classTotals = totals.LookUp (ct.Name);
        if  (classTotals == null)
        {
          classTotals = new ClassTotals (ct.Name, bucketCount, bucketSize);
          totals.Add (classTotals);
        }    
        classTotals.AddIn (ct);
      }
    }  /* SizeDistribution */



    /// <summary>
    /// Method used to giurantee that the specified class is included; even if none occur.
    /// </summary>
    /// <param name="mlClass">Class to add </param>
    public void   InitiateClass (PicesClass  mlClass)
    {
      if  (mlClass == null)
        mlClass = PicesClassList.GetUnKnownClassStatic ();

      ClassTotals  classTotals = totals.LookUp (mlClass.Name);
      if  (classTotals == null)
      {
        classTotals = new ClassTotals (mlClass.Name, bucketCount, bucketSize);
        totals.Add (classTotals);
      }    
      return;
    }  /* InitiateClass */




    public void   Increment (PicesClass  mlClass,
                             int         size
                            )
    {
      if  (mlClass == null)
        mlClass = PicesClassList.GetUnKnownClassStatic ();

      ClassTotals  classTotals = totals.LookUp (mlClass.Name);
      if  (classTotals == null)
      {
        classTotals = new ClassTotals (mlClass.Name, bucketCount, bucketSize);
        totals.Add (classTotals);
      }    

      classTotals.Increment (size);
    }  /* Increment */

    
 
    public  void     PrintFormatedDistributionMatrix (System.IO.StreamWriter  o)
    {
      totals.SortByName ();

      PrintFormatedHeader (o);

      ClassTotals  classTotals = null;

      ClassTotals  grandTotals = new ClassTotals ("Grand Totals", bucketCount, bucketSize);

      int  idx;

      for  (idx = 0;  idx < totals.Count;  idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintFormatedLine (o);
        grandTotals.AddIn (classTotals);
      }

      o.WriteLine ();
      grandTotals.PrintFormatedLine (o);
    }  /* PrintFormatedDistributionMatrix */



    public  void     PrintCSVDistributionMatrix (System.IO.StreamWriter  o)
    {
      totals.SortByName ();

      PrintCSVHeader (o);

      ClassTotals  classTotals = null;

      ClassTotals  grandTotals = new ClassTotals ("Grand Totals", bucketCount, bucketSize);

      int  idx;

      for  (idx = 0;  idx < totals.Count; idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintCSVLine (o);
        grandTotals.AddIn (classTotals);
      }

      o.WriteLine ();
      grandTotals.PrintCSVLine (o);
    }  /* PrintCSVDistributionMatrix */





    public  void   PrintTabDelDistributionMatrix (System.IO.StreamWriter  o)
    {
      totals.SortByName ();

      PrintTabDelHeader (o);

      ClassTotals  classTotals = null;

      ClassTotals  grandTotals = new ClassTotals ("Grand Totals", bucketCount, bucketSize);

      int  idx;

      for  (idx = 0;  idx < totals.Count;  idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintTabDelLine (o);
        grandTotals.AddIn (classTotals);
      }   
      o.WriteLine ();
      grandTotals.PrintTabDelLine (o);
    }  /* PrintTabDelDistributionMatrix */




    public void  PrintByClassCollumns (System.IO.StreamWriter        o,
                                       List<uint>                    scanLinesPerMeterDepth,
                                       List<InstrumentStatsByDepth>  volumePerMeterDepth,
                                       bool                          printDensity,            /**< Indicates to print density rather than counts. */
                                       PicesClassList                summarizeClasses         /**< List of classes that we need to pront summary columns for. */
                                      )  

    {
      PicesClassList  classes = BuildMLClassList ();
      int  cIDX;
 
      // Find the first and last buckets with activity
  
      int  firstBucket = -1;
      int  lastBucket = 0;
  
      for  (int bucketIDX = 0;  bucketIDX < bucketCount;  bucketIDX++)
      {
        int  bucketTotal = 0;
        for  (cIDX = 0;  cIDX < classes.Count;  cIDX++)
        {
          PicesClass   mlClass = classes[cIDX];
          ClassTotals  classTotals = totals.LookUp (mlClass.Name);
          bucketTotal += classTotals.BucketCount (bucketIDX);
        }

        if  (bucketTotal > 0)
        {
          if  (firstBucket < 0)
             firstBucket = bucketIDX;
          lastBucket = bucketIDX;
        }
	    }

      if  (firstBucket < 0)
      {
        o.WriteLine ();
        o.WriteLine ();
        o.WriteLine ("SizeDistribution::PrintByClassCollumns     *** There is no SizeDistribution Data ***");
        o.WriteLine ();
        return;
      }

      int[]  finalTotals = new int[classes.Count];
      int[]  summaryTotals = null;
      for  (int zed = 0;  zed < classes.Count;  zed++)
        finalTotals[zed] = 0;
      if  (summarizeClasses != null)
      {
        summaryTotals = new int[summarizeClasses.Count];
        for (int zed = 0; zed < summaryTotals.Length;  ++zed)
          summaryTotals[zed] = 0;
      }

      int  grandTotal = 0;

      String[] headLines = classes.ExtractThreeTitleLines ();

      String[] summaryHeadLines = null;
      if  (summarizeClasses == null)
      {
        summaryHeadLines = new String[3];
        summaryHeadLines[0] = "";
        summaryHeadLines[1] = "";
        summaryHeadLines[2] = "";
      }
      else
      {
        String[] temp = summarizeClasses.ExtractTwoTitleLines ();

        summaryHeadLines = new String[3];
        summaryHeadLines[2] = temp[1];
        summaryHeadLines[1] = temp[0];
        summaryHeadLines[0] = "";
        for  (int x = 0;  x < summarizeClasses.Count;  ++x)
        {
          if  (x > 0)
            summaryHeadLines[0] += "\t";
          summaryHeadLines[0] += "Summary";
        }
      }

      String s1 = "Abundance by Class";
      if  (printDensity)
        s1 = "Abundance/m-3";
      
      String  temperatureUOM      = PicesInstrumentData.TemperatureUnit;
      String  fluorescenceUOM     = PicesInstrumentData.FluorescenceUnit;
      String  densityUOM          = PicesInstrumentData.DensityUnit;
      String  oxygenUOM           = PicesInstrumentData.OxygenUnit;
      String  salinityUOM         = PicesInstrumentData.SalinityUnit;
      String  transmisivityUMO    = PicesInstrumentData.TransmisivityUnit;
      String  turbidityUMO        = PicesInstrumentData.TurbidityUnit;
      String  cdomFluorescenceUMO = PicesInstrumentData.CdomFluorescenceUnit;

      //   [O2 (ml/L) * 44.64]/1.027 = O2 umol/kg 

      o.WriteLine (""         + "\t" + ""      + "\t" + ""         + "\t" + s1);
      o.WriteLine (""         + "\t" + ""      + "\t" + "Volume"   + "\t" + headLines[0] + "\t" + "" + "\t" + ""        + "\t" + "" + "\t" + summaryHeadLines[0] + "\t" + "" + "\t" + s1);
      o.WriteLine (""         + "\t" + "Scan"  + "\t" + "filtered" + "\t" + headLines[1] + "\t" + "" + "\t" + "All"     + "\t" + "" + "\t" + summaryHeadLines[1] + "\t" + "" + "\t" + "Temperature"   + "\t" + "Salinity"  + "\t" + "Density"   + "\t" + "Fluorescence"  + "\t" + "FluorescenceSensor"  + "\t" + "Oxygen"  + "\t" + "Oxygen"   + "\t" + "Transmisivity"   + "\t" + "Turbidity"  + "\t" + "CdomFluorescence");
      o.WriteLine ("Depth(m)" + "\t" + "Lines" + "\t" + "m-3"      + "\t" + headLines[2] + "\t" + "" + "\t" + "Classes" + "\t" + "" + "\t" + summaryHeadLines[2] + "\t" + "" + "\t" + temperatureUOM  + "\t" + salinityUOM + "\t" + densityUOM  + "\t" + fluorescenceUOM + "\t" + "Volts"               + "\t" + oxygenUOM + "\t" + "umol/kg"  + "\t" + transmisivityUMO  + "\t" + turbidityUMO + "\t" + cdomFluorescenceUMO);

      ulong  totalScanLines = 0;
      double totalVolume    = 0.0f;

      int    imageSize = firstBucket * bucketSize;

      for  (int bucketIDX = firstBucket;  bucketIDX <= lastBucket;  bucketIDX++)
      {
        int  nextImageSize = imageSize + bucketSize;

        ulong  scanLinesDepthForThisBucket = 0;
        if  (scanLinesPerMeterDepth != null)
        {
          for  (int x = imageSize;  x < Math.Min (nextImageSize, scanLinesPerMeterDepth.Count);  x++)
            scanLinesDepthForThisBucket += scanLinesPerMeterDepth[x];
        }

        InstrumentStatsByDepth  totalThisBucketIdx = new InstrumentStatsByDepth (bucketIDX);
        double  volumeDepthForThisBucket = 0.0;
        if  (volumePerMeterDepth != null)
        {
          if  (bucketIDX < volumePerMeterDepth.Count)
            totalThisBucketIdx.Add (volumePerMeterDepth[bucketIDX]);
          for  (int x = imageSize;  x < Math.Min (nextImageSize, volumePerMeterDepth.Count);  x++)
          {
            if  (x < volumePerMeterDepth.Count)
              volumeDepthForThisBucket += volumePerMeterDepth[x].volumeSampled;
          }
        }
        totalThisBucketIdx.ComputeMean ();

        o.Write (imageSize.ToString ()                   + "\t" + 
                 scanLinesDepthForThisBucket.ToString () + "\t" + 
                 volumeDepthForThisBucket.ToString ("##,##0.000")
                );

        totalScanLines += scanLinesDepthForThisBucket;
        totalVolume    += volumeDepthForThisBucket;

        int  bucketTotal = 0;

        int  intIDX = 0;
        for  (cIDX = 0;  cIDX < classes.Count;  cIDX++)
        {
          PicesClass   mlClass  = classes[cIDX];
          ClassTotals  classTotals = totals.LookUp (mlClass.Name);

          int  qtyThisBucket = classTotals.BucketCount (bucketIDX);
          double  densityThisBucket = 0.0;
          if  (volumeDepthForThisBucket != 0.0)
            densityThisBucket = classTotals.BucketCount (bucketIDX) / volumeDepthForThisBucket;

          if  (printDensity)
            o.Write ("\t" + densityThisBucket.ToString ("###,##0.000"));
          else
            o.Write ("\t" + qtyThisBucket.ToString ());

          bucketTotal          += qtyThisBucket;
          finalTotals[intIDX]  += qtyThisBucket;
          grandTotal           += qtyThisBucket;

          intIDX++;
        }

        o.Write ("\t" + "");
        
        {
          if  (printDensity)
          {
            double  densityThisBucket = 0.0;
            if  (volumeDepthForThisBucket != 0.0)
              densityThisBucket = bucketTotal / volumeDepthForThisBucket;
            o.Write ("\t" + densityThisBucket.ToString ("###,##0.000"));
          }
          else
          {
            o.Write ("\t" + bucketTotal);
          }
        }

        o.Write ("\t" + "");

        if  ((summarizeClasses != null)  &&   (summarizeClasses.Count > 0))
        {
          int  zed = 0;
          foreach (PicesClass pc in  summarizeClasses)
          {
            int  qtyThisBucket = SumUpFamilyOfClassesForBucket (pc, bucketIDX);
            summaryTotals[zed] = qtyThisBucket;
            if  (printDensity)
            {
              double  densityThisBucket = 0.0;
              if  (volumeDepthForThisBucket != 0.0)
                densityThisBucket = qtyThisBucket / volumeDepthForThisBucket;
              o.Write ("\t" + densityThisBucket.ToString ("###,##0.000"));
            }
            else
            {
              o.Write ("\t" + qtyThisBucket.ToString ());
            }
            ++zed;
          }
        }

        o.Write ("\t" + "");

        if  (totalThisBucketIdx != null)
        {
          o.Write ("\t" + totalThisBucketIdx.temperatureMean.ToString ("#,##0.000")        +
                   "\t" + totalThisBucketIdx.salinityMean.ToString ("#,##0.000")           +
                   "\t" + totalThisBucketIdx.densityMean.ToString ("#,##0.000")            +
                   "\t" + totalThisBucketIdx.fluorescenceMean.ToString ("#,##0.000")       +
                   "\t" + totalThisBucketIdx.fluorescenceSensorMean.ToString ("#,##0.000") +
                   "\t" + totalThisBucketIdx.oxygenMean.ToString ("#,##0.000")             +
                   "\t" + totalThisBucketIdx.Oxygen_molPerKg.ToString ("#,##0.000")        +
                   "\t" + totalThisBucketIdx.transmisivityMean.ToString ("#,##0.000")      +
                   "\t" + totalThisBucketIdx.turbidityMean.ToString ("#,##0.000")          +
                   "\t" + totalThisBucketIdx.cdomFluorescenceMean.ToString ("#,##0.000")
                  );
        }

        imageSize = nextImageSize;
        o.WriteLine ();
	    }

      {
        o.WriteLine ();
        o.Write ("Final Totals" + "\t"  + totalScanLines + "\t" + totalVolume);
        for  (uint x = 0;  x < classes.Count;  x++)
        {
          double  density = 0;
          if  (totalVolume != 0.0)
            density  = finalTotals[x] / totalVolume;

          if  (printDensity)
            o.Write ("\t" + "");
          else
            o.Write ("\t" + finalTotals[x]);
        }

        o.Write ("\t");
        double  grandTotalDensity = 0.0;
        if  (totalVolume != 0.0)
          grandTotalDensity = grandTotal / totalVolume;

        if  (printDensity)
          o.Write ("\t" + grandTotalDensity.ToString ("###,##0.000"));
        else
          o.Write ("\t" + grandTotal.ToString ());

        o.Write ("\t");

        if  (summaryTotals != null)
        {
          for  (uint x = 0;  x < summaryTotals.Length; ++x)
          {
            o.Write ("\t");
            if  (printDensity)
            {
              if  (totalVolume != 0.0)
                o.Write ((summaryTotals[x] / totalVolume).ToString ("###,##0.000"));
              else
                o.Write (0.0);
            }
            else
            {
              o.Write (summaryTotals[x]);
            }
          }
        }

        o.WriteLine ();
      }

      classes = null;
    }  /* ReportByClassCollumns */




    private  PicesClassList  BuildMLClassList ()
    {
      PicesClassList  mlClasses = new PicesClassList ();

      int  cIDX;
      //for  (cIDX = 0;  cIDX <  totals->end ();  cIDX++)
      for  (cIDX = 0;  cIDX <  totals.Count;  cIDX++)
      {
        ClassTotals  ct = totals[cIDX];
        mlClasses.Add (PicesClassList.GetUniqueClass (ct.Name, ""));
      }

      mlClasses.SortByName ();

      return  mlClasses;
    }  /* BuildMLClassList */



    void  PrintCSVHeader (System.IO.StreamWriter  o)
    {
       o.Write ("\"Class Name\",Sum,");

       int  imageSize = 0;
       int  bucket;

       for (bucket = 0;  bucket <  (bucketCount - 1); bucket++)
       {
          imageSize = imageSize + bucketSize;
          o.Write ("," + imageSize.ToString ());
       }

       o.Write (",>" + imageSize.ToString ());
       o.WriteLine ();
    }  /* PrintCSVHeader */



    
    private  void  PrintFormatedHeader (System.IO.StreamWriter  o)
    {
      o.Write ("Class Name              TOTAL");
      int  imageSize = 0;
      int  bucket;

      for  (bucket = 0;  bucket <  (bucketCount - 1); bucket++)
      {
        imageSize = imageSize + bucketSize;
        String  sizeStr;
        sizeStr = imageSize.ToString ().PadLeft (8);
        o.Write (sizeStr);
      }

      String  s = ">" + imageSize.ToString ();
      s = s.PadLeft (8);
      o.Write (s);
      o.WriteLine ();

      o.Write ("==================      =====");

      for (bucket = 0;  bucket <  bucketCount; bucket++)
      {
         o.Write ("    ====");
      }
      o.WriteLine ();
    }  /* PrintFormatedHeader */



    private  void  PrintTabDelHeader (System.IO.StreamWriter  o)
    {
      o.Write ("\"Class Name\"" + "\t" + "Sum");

      int  imageSize = 0;
      int  bucket;

      for (bucket = 0;  bucket <  (bucketCount - 1); bucket++)
      {
         imageSize = imageSize + bucketSize;
         o.Write ("\t" + imageSize.ToString ());
      }

      o.Write ("\t" + ">" + imageSize.ToString ());

      o.WriteLine ();
    }  /* PrintTabDelHeader */
  }  /* SizeDistribution */
}

