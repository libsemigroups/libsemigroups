# gap -A --nointeract fundamental_K11n34_7.gap

Read("benchmark.g");
F := FreeGroup("a", "b", "c");
AssignGeneratorVariables(F);;
R := [ a^-1 * c * a * a * b^-1 * c * b * b * c, 
       b^-1 * c * b * b * c * a^-1 * c * a * a, 
       a * b^-1 * c * b * b * c * a^-1 * c * a, 
       a * a * b^-1 * c * b * b * c * a^-1 * c, 
       b * b * c * a^-1 * c * a * a * b^-1 * c, 
       b * c * a^-1 * c * a * a * b^-1 * c * b, 
       c * a^-1 * c * a * a * b^-1 * c * b * b, 
       c * a * a * b^-1 * c * b * b * c * a^-1, 
       c * b * b * c * a^-1 * c * a * a * b^-1, 
       a * a * c * a^-1 * b * c^-1 * b^-1 * b^-1 * a * c^-1 * a^-1 * a^-1 * b * b * c * b^-1 * c
];
G := F / R;
json := Benchmark("GAP", LowIndexSubgroups, [G, 7], 10);
fname := StringFormatted("json/{}-{}-{}-{}-fundamental-K11n34-7.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
