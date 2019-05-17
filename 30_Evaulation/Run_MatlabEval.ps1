﻿
########################################################################################################
######################                  Set base path                             ######################
########################################################################################################

$PSCommandPath
$PS_Main_path = $PSCommandPath | Split-Path -Parent

########################################################################################################
########################################################################################################


################################################################################################################################################################################################################
################################################################################################################################################################################################################


########################################################################################################
########   Process IONEX files (compared IONEX are the best candidates but not neccessarly) ############
########################################################################################################

$in_folder = 'C:\Users\lus2bp\Source\Repos\GINA\30_Evaulation\Runnables&Data\2018\comp_2ndHalf';
$out_Folder = 'C:\Users\lus2bp\Source\Repos\GINA\30_Evaulation\Runnables&Data\2018\comp_results_20190515_NightRun';
$ext = '18I';

$in_folder
$out_Folder

matlab -nosplash -noFigureWindows -wait -nodesktop -r "addpath(genpath('$PS_Main_path')); ext ='$ext'; in_folder='$in_folder'; out_Folder='$out_Folder'; IONEX_Compare_Main; quit"

########################################################################################################
########################################################################################################


################################################################################################################################################################################################################
################################################################################################################################################################################################################


########################################################################################################
################## Plot the processed IONEX files. Make statistic and create figs ######################
########################################################################################################

$in_folder = join-path -Path $out_Folder -ChildPath "data";
$out_folder_Base =    join-path -Path $in_folder -ChildPath "Stats"

$in_folder
$out_folder_Base

$fitGauss = 1;
$IntervalProbability = 0.0027;
$binNumberOfAllHist = 50;
$OnlyAllDataHist = 1;

$TECbool = 1;
$RMSbool = 1;
$MLHbool = 1;
$CH2bool = 1;

#matlab -nosplash -noFigureWindows -wait -nodesktop -r "addpath(genpath('$PS_Main_path')); TECbool = $TECbool; RMSbool = $RMSbool; MLHbool = $MLHbool; CH2bool = $CH2bool; fitGauss = $fitGauss; IntervalProbability = $IntervalProbability; binNumberOfAllHist = $binNumberOfAllHist; OnlyAllDataHist = $OnlyAllDataHist; in_folder='$in_folder'; out_folder_Base='$out_folder_Base'; PlotIONEXdiff_Main; quit"

########################################################################################################
########################################################################################################


################################################################################################################################################################################################################
################################################################################################################################################################################################################


########################################################################################################
######################                        Process .pl files                   ######################
########################################################################################################

#$pl_folder = 'C:\Users\lus2bp\Source\Repos\GINA\70_EGNOS_Project\core\files\DebugFiles\PL_Data';
#$pl_folder_out = 'C:\Users\lus2bp\Source\Repos\GINA\70_EGNOS_Project\core\files\DebugFiles\PL_Data\Results';
#matlab -nosplash -noFigureWindows -wait -nodesktop -r "addpath(genpath('$PS_Main_path')); pl_folder='$pl_folder'; pl_folder_out='$pl_folder_out'; ProtectionDataProcess_Main; quit"

########################################################################################################
########################################################################################################