if( -e $bin_path . "GtkModule.so" )
{
	system("cp -vp \"" . $bin_path . "GtkModule.so\" \"$instdir\"");
}
