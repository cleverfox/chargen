#!env perl

my $fontname="terminus";
#	$header = 1;
#	$suffix = ".psf";
#	$reject = 0;

open(BDF, "<$ARGV[0]") || die("$0: $ARGV[0]: $!\n");

while (<BDF>) {
    if (/^FONTBOUNDINGBOX\s+([0-9]+)\s+([0-9]+).*$/)
    {
        $width = $1;
        $height = $2;
    }
    elsif (/^CHARS\s+([0-9]+)$/)
    {
        $chars = $1;
        last;
    }
    if (/^FAMILY_NAME "([^"]+)"/) {
        $fontname=$1;
    };
#    if (/^PIXEL_SIZE\s+(\d+)$/) {
#        $height=$1;
#    };
}
$fontname=sprintf("%s_%dpt",$fontname,$height);

(defined($width) && defined($chars)) || die("$0: $ARGV[0]: missing width/height or CHARS\n");
# psf2 can handle almost anything, but there must be some reasonable limits
($width > 0 && $width <= 128) || die("$0: $ARGV[0]: width $width out of range\n");
($height > 0 && $height <= 256) || die("$0: $ARGV[0]: height $height out of range\n");
($chars > 0 && $chars <= 65536) || die("$0: $ARGV[0]: CHARS $chars out of range\n");
$header=1;
$minimal = $width != 8 || $height >= 256 || ($chars != 256 && $chars != 512);
$header != 0 || $minimal == 0 || die("$0: $ARGV[0]: RAW format: invalid width $width, height $height or CHARS $chars\n");
$chars == 256 || $reject == 0 || die("$0: $ARGV[0]: RAW format: CHARS $chars rejected, use -r to accept\n", $chars);

if (!defined($unicode)) { $unicode = $chars > 256; }
if (!defined($version)) { $version = $minimal; }
else { $version >= $minimal || die("$0: $ARGV[0]: requested version ", $version + 1,  ", required ", $minimal + 1, "\n"); }

$linesize = ($width + 7) >> 3;
$charsize = $linesize * $height;

sub int { $int = $!; }
sub bye
{
	close OUT;
	$output ne "-" && unlink($output) != 1 && print STDERR "$0: $output: $!\n";
	die("@_");
}

open(OUT, ">$fontname.c") || die("$0: $fontname $!\n");
$SIG{INT} = 'int';
binmode(OUT) || bye("$0: $fontname$!\n");

my $font;
my $hint;
my $width;
my $offset;
$lines = 0;
while (<BDF>)
{
	$bytes = 0;
	while (/^([0-9a-fA-F]{2})(([0-9a-fA-F]{2})*)$/)
	{
            push(@{$font->{$chid}}, sprintf "0x%02x,", hex($1));
            $bytes++;
            $_ = $2;
	}
	if ($bytes != 0)
	{
		$lines++;
		$bytes == $linesize || bye("$0: $ARGV[0]: invalid number of bytes $bytes on data line $lines\n");
	}
	else
	{
		if (/^STARTCHAR\s+(.+)$/) {
                    $chn=$1;
                };
                if (/^DWIDTH\s+([0-9]+)\s/) {
                    $width->{$chid}=$1;
                };
		if (/^ENCODING\s+([0-9]+)$/) {
                    $chid=$1;
                    $hint->{$chid}=sprintf(" /* %d %04X %s */",$1,$1,$chn);
                    $font->{$chid}=[];
                    #push @unimap, $1;
                    #if ($1 != 65535) { $unidup{$1}[0] = $1; }
                }
		elsif (/^ENDFONT$/) { last; }
	}
}

print OUT "#include \"fontdescr.h\"\n";
print OUT "const uint_8 ".$fontname."Bitmaps[] = {\n";

my @charset=(32..126,0x0401,0x0410..0x044f,0x0451);

my $pos=0;
for(@charset){
    $offset->{$_}=$pos;
    print OUT join('',@{$font->{$_}});
    $pos+=scalar @{$font->{$_}};
    print OUT $hint->{$_};
    print OUT "\n";
}
print OUT "};\n";

print OUT "const FONT_CHAR_INFO ".$fontname."Descriptors[] = {\n";
my $chn=32;
for(@charset){
    printf OUT "{%2d,%4d},",$width->{$_},$offset->{$_};
    printf OUT "%s /* %d */\n", $hint->{$_},$chn++;
}
print OUT "};\n";

printf OUT "const FONT_INFO %sFontInfo = {\n",$fontname;
printf OUT "\t%d, /*  Char height */\n",$height;
printf OUT "\t%d, /*  Start char */\n",$charset[0];
printf OUT "\t192, /*  End char */\n";
printf OUT "\t2, /*  space width*/\n";
printf OUT "\t%sDescriptors, /*  Char descr */\n",$fontname;
printf OUT "\t%sBitmaps, /*  Char bitmap */\n",$fontname;
print OUT "};\n";

close BDF;

close OUT || bye("$0: $fontname $!\n");
defined($int) && bye("$0: $int\n");

open(OUT, ">$fontname.h") || die("$0: $fontname $!\n");
printf OUT "extern const uint_8 %sBitmaps[];\n",$fontname;
printf OUT "extern const FONT_INFO %sFontInfo;\n",$fontname;
printf OUT "extern const FONT_CHAR_INFO %sDescriptors[];\n",$fontname;
close OUT || bye("$0: $fontname $!\n");

#32...126
#0410...042F A-YA
#0430...044f a-ya
#0401 YO
#0451 yo

