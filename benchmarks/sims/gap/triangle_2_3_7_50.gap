# gap -A --nointeract triangle_2_3_7_50.gap

Read("benchmark.g");
F := FreeGroup("a", "b");
AssignGeneratorVariables(F);;
R := [
          a * a, 
          b * b * b, 
          a * b * a * b * a * b * a * b * a * b * a * b * a * b, 
         ];
G := F / R;
json := Benchmark("GAP", LowIndexSubgroups, [G, 50], 10);
fname := StringFormatted("json/{}-{}-{}-{}-triangle-2-3-7-50.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
