use strict;
use warnings;

###########
# setting
###########

my @proto_dirs = ("WINNERLib");

foreach my $proto_dir(@proto_dirs)
{
	# get proto file list
	my @proto_files;
	open( DIR, "dir /B $proto_dir\\*.proto |");
	while(<DIR>)
	{
		chomp;
		push @proto_files, $_;
	}
	close(DIR);

	# compile proto files
	my $cmd = "protoc --proto_path =.\\;Z:\\include\\ --cpp_out=.\\ $proto_dir\\%s";
	print "Searching: $proto_dir\n";
	for my $proto_file (@proto_files)
	{
		my $cmd_line = sprintf($cmd, $proto_file);
		print "\t$cmd_line\n";
		system($cmd_line);
	}
}