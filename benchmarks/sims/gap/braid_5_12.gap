# gap -A --nointeract braid_5_12.gap

Read("benchmark.g");
F := FreeGroup("a", "b", "c", "d");
AssignGeneratorVariables(F);;
R := [
          [a * b * a, b * a * b],
          [b * c * b, c * b * c],
          [c * d * c, d * c * d],
          [a * c, c * a],
          [a * d, d * a],
          [b * d, d * b]
         ];
G := F / R;
json := Benchmark("GAP", LowIndexSubgroups, [G, 12], 10);
fname := StringFormatted("json/{}-{}-{}-{}-braid-5-12.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
