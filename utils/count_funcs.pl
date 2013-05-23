#!perl
open(F,$ARGV[0]) || die "$!";
@data = <F>;
close (F);

$count = 0;

foreach $i (@data) {
    if($i=~m/\{.*,.*\}/) {
	$count++
    }
}
print "#define N_FUNCS $count\n";
