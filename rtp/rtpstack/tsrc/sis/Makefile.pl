#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  
#

use Cwd ;
use File::Path;
use File::Copy;
use Time::Local;
use FileHandle;


$Project_Name="RTP_2.0";
$Package_Dir="\\WeeklyBuild_$Project_Name";
$Plat_Form="Epoc";
$TITLE="Build Script for $Project_Name";
our $MakeFileDir=cwd();
#$EPOC_ROOT="\\epoc32";
#$EPOC_DATA="$EPOC_ROOT\\data";
#$EPOC_BUILD="$EPOC_ROOT\\BUILD";
#$EPOC_DATA_PLUGINS="$EPOC_DATA\\z\\system\\Libs\\Plugins";
#$EPOC_RELEASE="$EPOC_ROOT\\release";
#$EPOC_FILE="\\epoc32\\wins\\c\\system";

$PACKGAGE_INST="$Package_Dir\\$Project_Name\\Inst\\$Plat_Form";
$BUILD_LOG="$Package_Dir\\$Project_Name\\Logs";
$SOURCE_CODE="$Package_Dir\\$Project_Name\\Src";

#@PLATFORM=("wins udeb");
@PLATFORM=("winscw udeb","armv5 urel");

@COMPONENT=("RTP_2.0");

my @BldInf=("..\\group");

########################## MAIN Program Starts #######################
&TitleText("$TITLE");
$Working_Dir=getcwd;
$Compile_Dir='';

#my @BldDir=&GetBldDir(@BldInf);
#my @Releaseable=&GetReleaseable(\@BldDir,"thumb","urel");
#&ActionFiles(\@Releaseable,"thumb","urel","Varify");
if(@ARGV==0)
  {

#    &Follow_Option(1);
    &Help;
  }
  else
    {
       eval("@ARGV[0]()") or warn "Wrong Parameter";
    }
#------------------------------------------------------------------------------#

 sub Help
  {
      chdir $MakefileDir;
      system('cls');
      print "*"x(150)."\n";
      Display();
      print "*\tOptions:\n";
      print "*\n";
      print "*\n";
      print "*\t\t\t(1)......Clean and Compile All $Project_Name Components  for @PLATFORM\n";
      print "*\n";
      print "*\t\t\t(2)......Make Complete Release Package (Strictly:- For SIP factory Personals)\n";
      print "*\n";
      print "*\t\t\t(3)......Cleans and Compile All $Project_Name Components For User Desired PlatForms  \n";
      print "*\n";
      print "*\t\t\t(q)......Quit \n";
      print "*\n";
      print "*\t\t\t Please Select the Appropriate Option like 1 2  with out brackets and press Enter----->" ;
      my $Option =<STDIN>;
      chomp $Option;
      Follow_Option($Option);
  }
#------------------------------------------------------------------------------#
sub Follow_Option
    {
      my ($Option)=@_;
      if ($Option==1 || $Option==2 || $Option==3 || $Option eq "q" || $Option==66 )
        {
          if($Option==1)
            {
              SIP_Components(1,"ALL");
              print "\n\n\n\n\n\n\nComplation Complete Enter to Continue -----";
              getc;
              &TitleText("$TITLE");
              Help();
            }
           if($Option==2)
            {
              &Package_Struct_Basic;
              &SIP_Components(2,"ALL");
              &Compile("winscw","udeb","..\\Internal\\test\\RtpUnitTest\\group",1);
              &BullsEye;
              &Dll_Stat;
              &SourceCode_Copy;
              print "\n\n\n\n\n\n\nComplation Complete Enter to Continue -----";
              getc;
              &TitleText("$TITLE");
              Help();
            }
           if($Option==3)
            {
              &SIP_Components();
              print "\n\n\n\n\n\n\nComplation Complete Enter to Continue -----";
              getc;
              &TitleText("$TITLE");
              Help();
            }

           if($Option eq "q" )
           {
              exit 0;
           }
           if($Option==66) #A Slight Hack, to create just  Release Package ,
              {
              SIP_Components(1,"ALL");
              #&BullsEye;
              #&Compile("winscw","udeb","..\\Internal\\test\\RtpUnitTest\\group");
              #&Package_Struct_Basic;
              #&SIP_Components(2,"ALL");
              #&Dll_Stat(@PLATFORM);
              #ackage_Struct();
              #Sis_File_Create();
              #Doxy_Docs();
              #Help();
              }
        }
        else
        {
          warn "Options to be Choosen are from 1,2,3 ans 4 All other values are illegal" ;
          print "\n------ Try Again Enter to Conitue-------";
          getc;
          Help();
        }
    }
#------------------------------------------------------------------------------#
sub SIP_Components
    {
      my $comp=0;
      my $plat=0;
      my $varient=0;
      my ($PkgFlag,$PlatFlag)=@_;
      my @PlatForm;
      my $Releseabls;
      if ($PlatFlag eq "ALL")
        {
          @PlatForm=@PLATFORM;
        }
        else
          {
            @PlatForm=&GetPlatForm;
          }


      #if(-d $EPOC_BUILD)
       #  {
        #  print" Deleted $EPOC_BUILD\n";
         # rmtree($EPOC_BUILD);
         #}
         $Compile_Dir = getcwd();

      foreach(@COMPONENT)
        {
         my $Component_Dir= $Compile_Dir;
         my $Plat_i=0;

         foreach(@PlatForm)
           {
            @_TempPlatform=split /\s+/,@PlatForm[$plat];
            $Plat_Form=@_TempPlatform[0];
            $Varient=@_TempPlatform[1];
                my $BFlag;
                if($Plat_Form eq "winscw")
                {
                  $BFlag=1
                  }
                  else
                    {
                      $BFlag=0
                      }

                $Releseabls=Compile("$Plat_Form","$Varient",$Compile_Dir,$BFlag);
                if ($PkgFlag==2)
                {
                  &Pkg_Enhance("$Plat_Form","$Varient");
                  &ActionFiles($Releseabls,$Plat_Form,$Varient,"Copy");
                  #&Copy_Files_Pkg("$Plat_Form","$Varient");
                }
                  $varient++;

              $plat++;
              $varient=0;
            }
            $comp++;
           }

        }
#------------------------------------------------------------------------------#
sub GetPlatForm
    {
      my $i=0;
      my $dsp=1;
      print "*"x(150)."\n" ;
      while($i<@PLATFORM)
        {
          print "\t ($dsp)............@PLATFORM[$i]\n";
          $i++;
          $dsp++;
        }
      print "\t ALL............ALL PLatForms\n\n";
      print "\n\n\t Please Choose the PlatForms You Need to Compile From Option above and press Enter--->";
      my $Option=<STDIN>;
      chomp $Option;
      if ($Option eq "ALL" || $Option eq "all")
        {
          return @PLATFORM    ;
        }
      if($Option<=$dsp && $Option>0)
        {
          #print "op=$Option";
          return @PLATFORM[$Option-1];
        }
          else
            {
              print "Valid Input are between 1 and ALL \n Try again.......";
              getc;
              GetPlatForm();
            }
    }
#------------------------------------------------------------------------------#
sub Compile
    {
       -e "$BUILD_LOG" or mkpath "$Package_Dir\\$Project_Name\\Logs",077;

       my ($PlatForm,$Varient,$Compile_Dir,$BullsFlag)= @_;
       my $Log_File="$PlatForm"."_"."$Varient"."."."log";
       my $CompCmd;
       my @EpocBld=&GetEpocBld($Compile_Dir);
       my($MakeFiles,$Releseabls)=GetReleaseable(\@EpocBld,$PlatForm,$Varient);
       #return ($Releseabls);
       my $TText;
       &ActionFiles($Releseabls,$PlatForm,$Varient,"Clean");
       my $tempstart=time;
       open LOG, ">>$BUILD_LOG\\$Log_File";
        if($BullsFlag==1)
        {
          my $ret=`cov01 -1`;
        }
       foreach my $ComMak (@{$MakeFiles})
       {
        $TText=&CompName($ComMak);
        &TitleText("Compiling $TText for $PlatForm $Varient"."      For Logs See File    $BUILD_LOG\\$Log_File");
        print LOG "*"x(120)."\n";
        print LOG "$TText LOG\n";
        print LOG "*"x(120)."\n";
        open PIPE ,"$ComMak 2>&1|";
        while( $tmp=<PIPE>)
        {
          print $tmp;
          print LOG $tmp;
        }
        close PIPE;
       }

       my $tempend=time;
       my $RetString=&CalculateTime($tempstart,$tempend);
       print LOG ("-")x200;
       print LOG "\n$RetString\n";

       close LOG;
       my $ret=`cov01 -0`;
       &ActionFiles($Releseabls,$PlatForm,$Varient,"Varify");
       return ($Releseabls);
       ($PlatForm,$Varient)= "";
    }
#-------------------------------------------------------------------------------
sub CompName
{
  my($CompName)=@_;
  my $r=substr($CompName,20);
#  $r=;
  @ar= (split /\\/,substr($r,1,-4));
  @pp=split /\./,@ar[-1];
  return $pp[0];
  
  print"";
  
}
#------------------------------------------------------------------------------#
sub Package_Struct_Basic
      {
        if (-d $Package_Dir)
          {
            printf "Deleting Previously Created Package...\n\n";
            rmtree( $Package_Dir) or die 'Cant delete $Package_Dir';
          }

        mkdir $Package_Dir,077  or die "Cant Make $Package_Dir";
        mkdir "$Package_Dir\\$Project_Name",077  or die "Cannot  Make dir $Package_Dir\\$Project_Name";
        mkdir "$Package_Dir\\$Project_Name\\Doc",077  or die "Cannot Make Dir $Package_Dir\\$Project_Name\\Doc";
        mkdir "$Package_Dir\\$Project_Name\\Inst",077  or die "Cannot Make Dir $Package_Dir\\$Project_Name\\Inst";
        mkdir "$Package_Dir\\$Project_Name\\Inst\\$Plat_Form",077  or die "Cannot Make Dir";
        mkdir "$Package_Dir\\$Project_Name\\Logs",077 or die "Some thing is wrong";
        mkdir "$Package_Dir\\$Project_Name\\Src",077  or die "Cannot Make Dir";
        print "\nPackage Structure Created Successfully....\n\n";
      }
#------------------------------------------------------------------------------#
sub Pkg_Enhance
    {
      my($PlatForm,$Varient)=@_;
      mkdir "$PACKGAGE_INST\\$PlatForm",0777 or die "Cannot  Make dir $PACKGAGE_INST\\$PlatForm";
      mkdir "$PACKGAGE_INST\\$PlatForm\\$Varient",0777 or die "Cannot  Make dir $PACKGAGE_INST\\$PlatForm\\$Varient";;
      ($PlatForm,$Varient)= "";
    }

#------------------------------------------------------------------------------#
sub SourceCode_Copy
    {
      chdir($Compile_Dir);
      print "Copying Source Code to $Package_Dir\\$Project_Name\\Src\n";
      system("xcopy/e/f/Q ..\\* $Package_Dir\\$Project_Name\\Src")==0 or die "Cannot Copy SIP Source Code";
    }
#------------------------------------------------------------------------------#
sub Dll_Stat
  {
  my @TPlat=("winscw udeb","armv5 urel");
  my $Thumb_Stat="$PACKGAGE_INST\\armv5\\UREL";
  my $Wins_Stat="$PACKGAGE_INST\\Winscw\\UDEB";
  my $Winscw_Stat="$PACKGAGE_INST\\Winscw\\UDEB";
  my $temp="delete.me";
  my $Total_T=0;             #For Size of Thumb Files
  my $Total_W=0;             #For Size of Wins Files
  my $Plat;
  my $Vart;
  my @XFiles;
  my $Fsize;
  my $Xfile;
  my $WinsUdeb;
  my $ThumbUrel;
  my $WinsCwUdeb;
  my $TWinsUdeb;
  my $TThumbUrel;
  my $TWinsCwUdeb;
  my $Counter=0;
  my $gettime;
  $OutFile="$Package_Dir\\$Project_Name\\Logs\\$Project_Name.txt";
  format Header=
          @<<<<<<< Project Statistics(In Bytes).
  $Project_Name
  
  
  FileName:                    Armv5 Urel            WinsCW Udeb
  -------------------------------------------------------------
.
  format Middle =
  @<<<<<<<<<<<<<<<<<<<<<<<<<<< @<<<<<<<               @<<<<<<
  $Xfile,                      $ThumbUrel             $WinsUdeb
.
  format Bottom=
  -------------------------------------------------------------
  Wins  Total:  @<<<<<<<<<
                $TWinsUdeb
  Thumb Total:  @<<<<<<
                $TThumbUrel
  Total Files:  @<<<<<<
                $Counter
  Create Date:  @<<<<<<<<<<<<<<<<<<<<<<<<<<<
                $gettime
  -------------------------------------------------------------
.

  if(-e "$Package_Dir\\$Project_Name\\Logs\\$Project_Name.txt")
    {
      unlink $temp;
    }
    open OUT,">$OutFile";
    
    #$~=Header;
    OUT->format_name("Header");
    write OUT;
    #print OUT;
    foreach my $PlatVart (@TPlat)
           {
            $Plat=(split / /,$PlatVart)[0];
            $Vart=(split / /,$PlatVart)[1];
            push @XFiles,`dir /b $PACKGAGE_INST\\$Plat\\$Vart\\*.dll`;
            push @XFiles,`dir /b $PACKGAGE_INST\\$Plat\\$Vart\\*.exe`;
            push @XFiles,`dir /b $PACKGAGE_INST\\$Plat\\$Vart\\*.app`;
            }
            my %out;
            @out{@XFiles}=() ;
            @XFiles= sort keys %out;
            $Counter=scalar @XFiles;
            foreach  $Xfile (@XFiles)
              {
                foreach my $PlatVart (@TPlat)
                 {
                  $Plat=(split / /,$PlatVart)[0];
                  $Vart=(split / /,$PlatVart)[1];
                  chomp $Xfile;
                  if(-e "$PACKGAGE_INST\\$Plat\\$Vart\\$Xfile")
                    {
                     $Fsize=(stat "$PACKGAGE_INST\\$Plat\\$Vart\\$Xfile")[7] ;
                     }
                  if( $Plat eq "wins" || $Plat eq "armv5" || $Plat eq "winscw")
                    {

                     if ( $Plat eq "winscw")
                     {
                      $WinsUdeb=$Fsize;
                      $TWinsUdeb=$Fsize+$TWinsUdeb;
                     }
                     if ( $Plat eq "armv5")
                     {
                      $ThumbUrel=$Fsize;
                      $TThumbUrel=$Fsize+$TThumbUrel;
                     }

                    }
                     $Fsize="-";

                  }
                 OUT->format_name("Middle");
                 #$~=Middle;
                 write OUT;
                }
      $gettime=scalar gmtime;
      OUT->format_name("Bottom");
       #$~=Bottom;
      write OUT;
      close OUT;
}

#------------------------------------------------------------------------------#
sub Display
  {
    my $dsp=1;
    print "*\tProject:\t$Project_Name\n*\n*\tComponents:\n";
    while($i<@COMPONENT)
        {
            print "*\t\t\t@COMPONENT[$i]\n";
            $i++;
            $dsp++;
        }
        #print "*\t\t\t UI=$UI_FLAG And PkgFile=$PKG_FILE;\n";

    $dsp=1;
    $i=0;
    print "*\n";
    print "*\tPlatForms:\n";
    while($i<@PLATFORM)
        {
            print "*\t\t\t@PLATFORM[$i]\n";
            $i++;
            $dsp++;
        }
  }

#------------------------------------------------------------------------------#
sub TitleText
  {
    @TitleText=@_;
    `title @TitleText`;
  }
#------------------------------------------------------------------------------#
sub DisplayMsg
  {
  my($Msg)=@_;
    print "*"x(120)."\n";
    print "\t$Msg\n";
    print "*"x(120)."\n";

  }


#------------------------------------------------------------------------------#
sub  CalculateTime
   {
   #$Sec=time;
    my($nstart,$nend)=@_;
    $difference = $nend - $nstart;
    $seconds    =  $difference % 60;
    $difference = ($difference - $seconds) / 60;
    $minutes    =  $difference % 60;
    $difference = ($difference - $minutes) / 60;
    $hours      =  $difference % 24;
    $difference = ($difference - $hours)   / 24;
    $days       =  $difference % 7;
    $weeks      = ($difference - $days)    /  7;
    $RetString="Compilation Time: $hours Hours ,$minutes Minutes and $seconds Seconds";
    return $RetString;
   }
#------------------------------New Subroutines------------------------------------------#
#---------------------------------------------------------------------------#
sub ActionFiles
{
  my($DelVr,$Plat,$Vart,$Action)=@_;
  my @Deliverable=@{$DelVr};
  if($Action eq "Varify" || $Action eq "Copy" || $Action eq "Clean")
    {
      if($Action eq "Varify")
        {
          print "\n\nVarifying Generated Files Status \n";
          print "-"x(120)."\n";
          foreach my $DllFile (@Deliverable)
            {
              if (-e $DllFile)
                {
                  my $ttt=substr($DllFile,-3);

                  print "$DllFile........OK............$ttt\n" ;#if ($ttt eq "DLL" || $ttt eq "EXE" || $ttt eq "APP")  ;
                }
              else
                {
                  FatalError("Varifying Compiled File Status:","File: $DllFile Not Generated")
                }
             }
             print "-"x(120)."\n";
        }
      if($Action eq "Clean")
        {
          print "\n\nCleanning ALL Previously Generated Files \n";
          print "-"x(120)."\n";
          foreach my $DllFile (@Deliverable)
            {
              if (-e $DllFile)
                {
                  unlink $DllFile;
                  print "$DllFile........Cleaned\n";
                }
              else
                {
                  print "Unable To Find $DllFile, \n";
                }
             }
             print "-"x(120)."\n";
        }
       if($Action eq "Copy")
        {
          my $ZipFile="$PACKGAGE_INST\\$Plat\\$Vart\\$Plat-$Vart.zip";
          my $tempFile="$PACKGAGE_INST\\$Plat\\$Vart\\$Plat-$Vart.txt";
          my $ZipCommand="wzzip -r -P  $ZipFile \@$tempFile";
          open TempFile,">$tempFile" or "&FatalError($tempFile cannot be opened";
          print "\n\nCopying and Zipping Generated Files To Release Package\n";
          print "-"x(120)."\n";
          (-d  "$PACKGAGE_INST\\$Plat\\$Vart") or FatalError("Checking","$PACKGAGE_INST\\$Plat\\$Vart");
          foreach my $DllFile (@Deliverable)
            {
              if (-e $DllFile)
                {
                  copy($DllFile,"$PACKGAGE_INST\\$Plat\\$Vart") if (substr($DllFile,-3) eq "dll" || substr($DllFile,-3) eq "EXE" || substr($DllFile,-3) eq "APP" || substr($DllFile,-3) eq "lib") ;
                  print "$DllFile........Copied\n";
                  print TempFile "$DllFile\n";
                }
              else
                {
                  print "Unable To Find $DllFile, \n";
                }
             }
             system("$ZipCommand");
             print "-"x(120)."\n";
             close TempFile;
             unlink  $tempFile;
        }
    }
    else
      {
        &FatalError("UnAcceptable Parameter $Action:",
        "You tried to pass a parameter which is not Acceptable");
      }
}

#---------------------------------------------------------------------------#
sub GetEpocBld
{
  #my (@BldInf)=@_;
  my @BldInf=("..\\Group");
  my $BD=cwd();
  my $InfDir;
  my @BuildDirList;
  my $Epoc="\\epoc32";
  if (-d ! $Epoc)
  {
  &FatalError("$Epoc Directory not found");
  }
  foreach my $Inf (@BldInf)
  {
    chomp $Inf;
    if($Inf eq ".")
    {
    $Inf=cwd();
    }
    if (-d $Inf)
    {
     chdir $Inf==1 or &FatalError("Cannot Chage Dir  to $Inf");
     $PWD=cwd();


     @PWD=split /:/,$PWD;
     $PWD[0]="$Epoc\\build";
     $PWD=join '\\',@PWD;
#     chdir $PWD==1 or &FatalError("Cannot Chage Dir  to $Inf");
     #chdir $BD;
     push @BuildDirList,$PWD;
    }
    else
    {
      &FatalError ("$Inf Dir does not exist Cannot Continue");
    }
  }
  return @BuildDirList;
}
#---------------------------------------------------------------------------#

sub GetReleaseable
    {
      my($BuildDir,$Plat,$Vart)=@_;
      my @BldDir=@{$BuildDir};
      my $MakeString;
      my $MakeCommand="make";
      my @Releseable;
      my $MakePar1="";
      my @RES;
      my $GenMakeFile;
      my @CompCmd;
      my $Clean;
      my $Silent="";

    print "Generating List of Deliverables\n";
    if($Plat eq "wins")
      {
        $MakeCommand="nmake";
        $MakePar1="-nologo";
        $Silent="-s";
      }

      foreach my $BuildDir (@BldDir)
      {
         if (-d $BuildDir)
         {
          rmtree ($BuildDir);
          }
         -e "bld.inf" or die "Cannot Find Bld.inf FIle";
         @Bldtemp=`bldmake bldfiles 2>&1`;
         unless (@Bldtemp==0)
          {
            &FatalError("@Bldtemp, From $Inf");
          }
         if(-e "$BuildDir\\Export.make")
            {
              my $ret=`make -f $BuildDir\\Export.make`;
            }
          $MakeString="$BuildDir\\$Plat.Make";
         $Clean=`make -f $MakeString CLEANMAKEFILE`;
         system("make -f $MakeString MAKEFILE");
         $Resable=`make -f $MakeString WHATMAKEFILE`; # CFG=$Vart`;

         
         $Resable=~s/"//g;
         $Resable=~s/\n/ /g;
        @Res=split / /,$Resable;
        @Res=   grep (!/CPP/,@Res);
        if(-e "$BuildDir\\$Plat.make")
          {
            my $MAKE="make -f $BuildDir\\$Plat.make MAKEFILE ";
            #@OO=`$MAKE 2>&1`;
            #print "$MAKE ,@OO\n";
          }
        else
          {
          &FatalError("MakeFile" ,"Not Found $BldFile\\$Plat.make");
          }
        foreach my  $ComMakefile (@Res)
        {
          $MakeString="$MakeCommand $MakePar1 -f  $ComMakefile WHAT$Vart";
          $Resable=`$MakeString 2>&1`;
          $MakeString="$MakeCommand $MakePar1 $Silent -f  $ComMakefile $Vart";
          if($Resable=~/Stop/)
          {
           &FatalError("Genterating Releaseable Error->","$Resable"); # CFG=$Vart`;
           }
          $Resable=~s/"//g;
          $Resable=~s/\n/ /g;
          @Releseable=split / /,$Resable;
          print ".";
          push @CompCmd,$MakeString;
          push @RES,@Releseable;
        }
       #$BuildInfo{$Plat}{$Vart}=[@RES];
      }
      print "Done\n";
      return (\@CompCmd,\@RES);
    }
#---------------------------------------------------------------------------#
sub FatalError
   {
   my($Heading,$Description)=@_;
   print "ERROR:\n$Heading $Description\n"  ;
	 exit 1;
   }
#---------------------------------------------------------------------------#

sub BullsEye
  {
    my $ret=`\\epoc32\\release\\winscw\\udeb\\ATSINTERFACE.EXE -testmodule RtpUnitTest`  ;
    my $ret=`covsrc > "$Package_Dir\\$Project_Name\\Logs\\BullsEye.txt`;

  }
