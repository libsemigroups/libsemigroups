# gap -A --nointeract fundamental_K15n12345.gap

Read("benchmark.g");
F := FreeGroup("a", "b", "c");
AssignGeneratorVariables(F);;
R := [
          a^-1 * c^-1 * a^-1 * c * b * a * b^-1 * c, 
          a^-1 * c * b * a * b^-1 * c * a^-1 * c^-1, 
          b^-1 * c * a^-1 * c^-1 * a^-1 * c * b * a, 
          c^-1 * a^-1 * c * b * a * b^-1 * c * a^-1, 
          a * b^-1 * c * a^-1 * c^-1 * a^-1 * c * b, 
          b * a * b^-1 * c * a^-1 * c^-1 * a^-1 * c, 
          c * a^-1 * c^-1 * a^-1 * c * b * a * b^-1, 
          c * b * a * b^-1 * c * a^-1 * c^-1 * a^-1, 
          a * b^-1 * a * c^-1 * a * c * b^-1 * a^-1 * c * a^-1 * b * a * b^-1 * a * b * a * c^-1
* a^-1 * c * a^-1 * b * a * b^-1 * a * c^-1 * a * c * b^-1 * a^-1 * c * a^-1 * b * a * b^-1 * a * b *
c^-1 * a^-1 * c * a^-1 * b * a^-1 * b^-1 * a * c^-1 * a * b * a^-1 * b^-1 * a^-1 * b * a^-1 * b^-1 * a * c^-1 * a
* b * c^-1 * a^-1 * c * a^-1 * b
         ];
G := F / R;
json := Benchmark("GAP", LowIndexSubgroups, [G, 7], 10);
fname := StringFormatted("json/{}-{}-{}-{}-fundamental-K15n12345-7.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
