using System;
using System.Collections.Generic;
using System.Text;

using  PicesInterface;


/**
 *@brief  Cloned from "SizeDistribution2" with the intent to use for Image Size in mm^2  a floating point number.
 *@details The sizing will range by a factor with a starting value and growth rate.
 */

namespace PicesCommander
{

//#define  BucketCount  50
//#define  BucketSize   100


  public  class  SizeDistribution2 
  {
   
    public  class  ClassTotals2  
    {
      int      bucketCount = 0;
      int      count       = 0;
      String   name        = null;
      String   nameUpper   = null;
      
      float    sizeInitial      = 0.1f; /**<  Starting size; all following buckest will grow by growth factor. */
      float    sizeGrowthFactor = 1.2f; 
      float    sizeEndRange     = 10.0f;
      float[]  sizeBucketStart  = null; /**<  the start of the size range for the respective bucket. */
      float[]  sizeBucketEnd    = null; /**<  the end of the size range for the respective bucket.   */
      int[]    sizeBuckets      = null;


      public  ClassTotals2 (String   _name,
                            float    _sizeInitial,
                            float    _sizeGrowthFactor,
                            float    _sizeEndRange,
                            int      _bucketCount,
                            float[]  _sizeBucketStart,
                            float[]  _sizeBucketEnd
                          )
      {
        count            = 0;
        name             = _name;
        nameUpper        = _name.ToUpper ();
        sizeInitial      = _sizeInitial;
        sizeGrowthFactor = _sizeGrowthFactor;
        sizeEndRange     = _sizeEndRange;
        bucketCount      = _bucketCount;
        sizeBucketStart  = _sizeBucketStart;
        sizeBucketEnd    = _sizeBucketEnd;
        sizeBuckets      = new int[bucketCount];
        for  (int x = 0;  x < bucketCount;  ++x)
          sizeBuckets[x] = 0;
        count = 0;
      }


      public  String  Name              {get  {return name;}}
      public  String  NameUpper         {get  {return nameUpper;}}
      public  float   SizeEndRange      {get  {return sizeEndRange;}}
      public  float   SizeGrowthFactor  {get  {return sizeGrowthFactor;}}
      public  float   SizeInitial       {get  {return sizeInitial;}}


      public  void  Increment (float  areaMM)
      {
        int  bucket = -1;
        int  left  = 0;
        int  right = sizeBucketStart.Length - 1;

        while  (bucket < 0)
        {
          int  m = (left + right) / 2;
          if  (areaMM < sizeBucketStart[m])
            right = m - 1;
          else if  (areaMM >= sizeBucketEnd[m])
            left = m + 1;
          else
            bucket = m;
        }

        if  (bucket >= bucketCount)
          bucket = bucketCount - 1;

        if  (bucket < 0)
          bucket = 0;

        sizeBuckets[bucket]++;
        count++;
      }



      public  void  AddIn (ClassTotals2  classTotals)
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



      public  void  PrintTabDelLine (System.IO.StreamWriter  o,
                                     float                   volume
                                    )
      {
        o.Write ("\"" + name + "\"" + "\t" + count.ToString ());

        int  bucket;

        for (bucket = 0;  bucket <  bucketCount; bucket++)
        {
          if  (volume == 0.0f)
            o.Write ("\t" + sizeBuckets[bucket].ToString ());
          else
          {
            float density = (float)sizeBuckets[bucket] / volume;
            o.Write ("\t" + density.ToString ("##,##0.000"));
          }
        }

        o.WriteLine ();
      }
    };  /* PrintTabDelLine */




    class  ClassTotals2List:  List<ClassTotals2>
    {
      public  ClassTotals2List ()
      {
      }
      

   
      public  ClassTotals2  LookUp (String  _name)
      {
        _name = _name.ToUpper ();

        int  idx;
        ClassTotals2  classTotals = null;
        ClassTotals2  temp        = null;

        for  (idx = 0; (idx < Count)  &&  (classTotals == null);  idx++)
        {
          temp = (this)[idx];
          if  (temp.NameUpper == _name)
            classTotals = temp;
        }

        return  classTotals;
      }  /* LookUp */



      private class  ClassTotals2Comparator:  IComparer<ClassTotals2>
      {
        int  IComparer<ClassTotals2>.Compare (ClassTotals2 x, ClassTotals2 y)  
        {
          return x.Name.CompareTo (y.Name);
        }
      }  /* ClassTotals2Comparator */



      public  void  SortByName ()
      {
        Sort (new ClassTotals2Comparator ());
      }

    };  /* ClassTotals2List */

    
    float    sizeInitial      = 0.1f; /**<  Starting size; all following buckest will grow by growth factor. */
    float    sizeGrowthFactor = 1.2f; 
    float    sizeEndRange     = 10.0f;
    float[]  sizeBucketStart  = null; /**<  the start of the size range for the respective bucket. */
    float[]  sizeBucketEnd    = null; /**<  the end of the size range for the respective bucket.   */
    int[]    sizeBuckets      = null;
    int      bucketCount      = 0;
    ClassTotals2List  totals  = null;



    public  SizeDistribution2 (float  _sizeInitial,
                               float  _sizeGrowthFactor,
                               float  _sizeEndRange
                              )
    {
      sizeInitial      = _sizeInitial;
      sizeGrowthFactor = _sizeGrowthFactor;
      sizeEndRange     = _sizeEndRange;

      {
        // Compute number of buckets needed.
        bucketCount = 0;
        ++bucketCount; // For images less than "sizeInitial".

        float sizeBegin = _sizeInitial;
        while  (sizeBegin <= sizeEndRange)
        {
          sizeBegin = sizeBegin * sizeGrowthFactor;
          sizeBegin = (float)Math.Round (sizeBegin, 4);
          ++bucketCount;
        }

        ++bucketCount;  // For images that are larger than "sizeEndRange".
      }

      sizeBuckets     = new int  [bucketCount];
      sizeBucketStart = new float[bucketCount];
      sizeBucketEnd   = new float[bucketCount];

      {
        // Initialize Bucket arrays.
        int x = 0;
        sizeBucketStart[x] = 0.0f;
        sizeBucketEnd  [x] = sizeInitial;
        sizeBuckets    [x] = 0;
        ++x;

        float sizeBegin = sizeInitial;
        while  (sizeBegin <= sizeEndRange)
        {
          float  sizeEndOfRange = sizeBegin * sizeGrowthFactor;
          sizeEndOfRange = (float)Math.Round (sizeEndOfRange, 4);
          sizeBucketStart[x] = sizeBegin;
          sizeBucketEnd  [x] = sizeEndOfRange;
          sizeBuckets    [x] = 0;
          ++x;
          sizeBegin = sizeEndOfRange;
        }

        sizeBucketStart[x] = sizeBegin;
        sizeBucketEnd  [x] = 99999.9999f;;
        sizeBuckets    [x] = 0;
        ++x;
      }

      totals = new ClassTotals2List ();
    }


    /// <summary>
    /// Returns an instance of 'ClassTotals' that contains a summary of the classes that are decedent of 'ancestor'.
    /// </summary>
    /// <param name="ancestor"></param>
    /// <returns></returns>
    public  ClassTotals2  SummarizeFamilyOfClasses (PicesClass  ancestor)
    {
      ClassTotals2  familySummary = new ClassTotals2 (ancestor.Name, sizeInitial, sizeGrowthFactor,sizeEndRange, bucketCount, sizeBucketStart, sizeBucketEnd);
      AddFamilyOfClassesToSizeClassTotals (familySummary, ancestor);
      return   familySummary;
    }  /* SummarizeFamilyOfClasses */




    private  void  AddFamilyOfClassesToSizeClassTotals (ClassTotals2   summary,
                                                        PicesClass     ancestor
                                                       )
    {
      ClassTotals2  ct = totals.LookUp (ancestor.Name);
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
    /// Returns an instance of 'ClassTotals' that contains the ClassTotals that belong to the family of 'ancestor'.
    /// The newly instantiated instance will point to the same instances of 'ClassTotals'  that this instance points to.
    /// As a result you should only call this method after you are done accumulating data.
    /// </summary>
    /// <param name="ancestor"></param>
    /// <returns></returns>
    public  SizeDistribution2  ExtractFamilyOfClasses (PicesClass  ancestor)
    {
      SizeDistribution2  family = new SizeDistribution2 (sizeInitial, sizeGrowthFactor, sizeEndRange);
      ExtractFamily (family, ancestor);
      return   family;
    }  /* ExtractFamilyOfClasses */




    private  void  ExtractFamily (SizeDistribution2   family,
                                  PicesClass          ancestor
                                 )
    {
      ClassTotals2  ct = totals.LookUp (ancestor.Name);
      if  (ct != null)
        family.Add (ct);
      
      if  (ancestor.Children != null)
      {
        foreach  (PicesClass  pc in ancestor.Children)
          ExtractFamily (family, pc);
      }

      return;
    }  /* ExtractFamily */





  

    /// <summary>
    /// Will add in the contents of another SizeDistribution2 instance into this instance.
    /// </summary>
    /// <param name="x">The other 'SizeDistribution2' instance to be accumulated to this instance.</param>
    public  void  Add (SizeDistribution2  x)
    {
      if  ((x.bucketCount       != this.bucketCount)      ||
           (x.sizeInitial       != this.sizeInitial)      ||
           (x.sizeGrowthFactor  != this.sizeGrowthFactor) ||
           (x.sizeEndRange      != this.sizeEndRange)
          )
      {
        throw new Exception ("'SizeDistribution2.Add   ***ERROR***   Dimensions are not the same" + "\n" +
                             "Left.sizeInitial      [" + sizeInitial.ToString      () + "]  Right [" + x.sizeInitial.ToString      () + "]" + "\n" +
                             "Left.sizeGrowthFactor [" + sizeGrowthFactor.ToString () + "]  Right [" + x.sizeGrowthFactor.ToString () + "]" + "\n" +
                             "Left.sizeEndRange     [" + sizeEndRange.ToString     () + "]  Right [" + x.sizeEndRange.ToString     () + "]"
                             );
      }

      foreach (ClassTotals2  ct in x.totals)
      {
        ClassTotals2  classTotals = totals.LookUp (ct.Name);
        if  (classTotals == null)
        {
          classTotals = new ClassTotals2 (ct.Name,
                                          sizeInitial,
                                          sizeGrowthFactor,
                                          sizeEndRange, 
                                          bucketCount, 
                                          sizeBucketStart, 
                                          sizeBucketEnd
                                         );
          totals.Add (classTotals);
        }    
        classTotals.AddIn (ct);
      }
    }  /* SizeDistribution2 */



    //
    /// <summary>
    /// Will add in the contents of a ClassTotals2 instance into this instance of SizeDistribution2.
    /// </summary>
    /// <param name="x"></param>
    public  void  Add (ClassTotals2  x)
    {
      if  ((x.SizeInitial       != this.sizeInitial)      ||
           (x.SizeGrowthFactor  != this.sizeGrowthFactor) ||
           (x.SizeEndRange      != this.sizeEndRange)
          )
      {
        throw new Exception ("'SizeDistribution2.Add   ***ERROR***   Dimensions are not Compatible" + "\n" +
                             "Left.sizeInitial      [" + sizeInitial.ToString      () + "]  Right [" + x.SizeInitial.ToString      () + "]" + "\n" +
                             "Left.sizeGrowthFactor [" + sizeGrowthFactor.ToString () + "]  Right [" + x.SizeGrowthFactor.ToString () + "]" + "\n" +
                             "Left.sizeEndRange     [" + sizeEndRange.ToString     () + "]  Right [" + x.SizeEndRange.ToString     () + "]"
                             );
      }

      totals.Add (x);
    }  /* SizeDistribution2 */









    /// <summary>
    /// Method used to guarantee that the specified class is included; even if none occur.
    /// </summary>
    /// <param name="mlClass">Class to add </param>
    public void   InitiateClass (PicesClass  mlClass)
    {
      if  (mlClass == null)
        mlClass = PicesClassList.GetUnKnownClassStatic ();

      ClassTotals2  classTotals = totals.LookUp (mlClass.Name);
      if  (classTotals == null)
      {
        classTotals = new ClassTotals2 (mlClass.Name, 
                                        sizeInitial,
                                        sizeGrowthFactor,
                                        sizeEndRange,
                                        bucketCount, 
                                        sizeBucketStart, 
                                        sizeBucketEnd
                                       );
        totals.Add (classTotals);
      }    
      return;
    }  /* InitiateClass */




    public void   Increment (PicesClass  mlClass,
                             float       areaMM
                            )
    {
      if  (mlClass == null)
        mlClass = PicesClassList.GetUnKnownClassStatic ();

      ClassTotals2  classTotals = totals.LookUp (mlClass.Name);
      if  (classTotals == null)
      {
        classTotals = new ClassTotals2 (mlClass.Name, 
                                        sizeInitial,
                                        sizeGrowthFactor,
                                        sizeEndRange,
                                        bucketCount, 
                                        sizeBucketStart, 
                                        sizeBucketEnd
                                       );
        totals.Add (classTotals);
      }    

      classTotals.Increment (areaMM);
    }  /* Increment */

 

    /// <summary>
    /// 
    /// </summary>
    /// <param name="o">Output stream to print report to.</param>
    /// <param name="volume">If volume == 0.0 then prints units otherwise prints the density units/mm^3. </param>
    public  void   PrintTabDelDistributionMatrix (System.IO.StreamWriter  o,
                                                  float                   volume
                                                 )
    {
      totals.SortByName ();

      PrintTabDelHeader (o);

      ClassTotals2  classTotals = null;

      ClassTotals2  grandTotals = new ClassTotals2 ("Grand Totals",
                                                    sizeInitial,
                                                    sizeGrowthFactor,
                                                    sizeEndRange,
                                                    bucketCount,
                                                    sizeBucketStart,
                                                    sizeBucketEnd
                                                   );

      int  idx;

      for  (idx = 0;  idx < totals.Count;  idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintTabDelLine (o, volume);
        grandTotals.AddIn (classTotals);
      }   
      o.WriteLine ();
      grandTotals.PrintTabDelLine (o, volume);
    }  /* PrintTabDelDistributionMatrix */






    public  void   PrintTabDelDistributionMatrixesForSummaryClasses (System.IO.StreamWriter  o,
                                                                     String                  majorTitle,
                                                                     float                   volume
                                                                    )
    {
      PicesClassList  allKnownClasses = PicesClassList.GetAllKnownClasses ();
      PicesClassList  allSummaryClasses = allKnownClasses.ExtractSummarizeClasses ();
      allSummaryClasses.SortByName ();

      foreach  (PicesClass pc in allSummaryClasses)
      {
        SizeDistribution2  familySummary = this.ExtractFamilyOfClasses (pc);
        if  (familySummary.totals.Count > 1)
        {
          o.WriteLine (majorTitle + "      *** Summary of " + pc.Name + " ***");
          o.WriteLine ();
          familySummary.PrintTabDelDistributionMatrix (o, volume);
          o.WriteLine ();
          o.WriteLine ();
        }
      }
    }  /* PrintTabDelDistributionMatrixesForSummaryClasses */



    private  PicesClassList  BuildMLClassList ()
    {
      PicesClassList  mlClasses = new PicesClassList ();

      int  cIDX;
      //for  (cIDX = 0;  cIDX <  totals->end ();  cIDX++)
      for  (cIDX = 0;  cIDX <  totals.Count;  cIDX++)
      {
        ClassTotals2  ct = totals[cIDX];
        mlClasses.Add (PicesClassList.GetUniqueClass (ct.Name, ""));
      }

      mlClasses.SortByName ();

      return  mlClasses;
    }  /* BuildMLClassList */


    
    private  void  PrintTabDelHeader (System.IO.StreamWriter  o)
    {
      o.Write ("\"Class Name\"" + "\t" + "Sum");

      int  bucket;

      for (bucket = 0;  bucket <  bucketCount; ++bucket)
      {
        String  sizeStr = sizeBucketStart[bucket].ToString () + " <= x < " + sizeBucketEnd[bucket].ToString ();
         o.Write ("\t" + sizeStr);
      }

      o.WriteLine ();
    }  /* PrintTabDelHeader */
  }  /* SizeDistribution2 */
}

