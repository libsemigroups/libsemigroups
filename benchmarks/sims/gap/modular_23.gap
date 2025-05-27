# gap -A --nointeract modular_23.gap

Read("benchmark.g");
F := FreeGroup("S", "T");
AssignGeneratorVariables(F);;
R := [S * S, (S * T) ^ 3, (T * S) ^ 3];
G := F / R;
json := Benchmark("GAP", LowIndexSubgroups, [G, 23], 10);
fname := StringFormatted("json/{}-{}-{}-{}-modular-23.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
