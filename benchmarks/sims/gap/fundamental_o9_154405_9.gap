# gap -A --nointeract fundamental_o9_154405_9.gap

Read("benchmark.g");
F := FreeGroup("a", "b");
AssignGeneratorVariables(F);;
R := [
          b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a
* a * b * b * b * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a *
a * a * a, 
          b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a
* b * b * b * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a *
a * a * b^-1, 
          b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b
* b * b * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a *
a * b^-1 * b^-1, 
          b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b
* b * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a *
b^-1 * b^-1 * b^-1, 
          b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b
* a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 *
b^-1 * b^-1 * b^-1, 
          b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a
* a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 *
b^-1 * b^-1 * b^-1, 
          b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a
* a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 *
b^-1 * b^-1 * b^-1, 
          b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a
* a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 *
b^-1 * b^-1 * b^-1, 
          a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b
* a * a * b * b * b * a * a * a * a * a * b * b * b * a * a * b * b * b * a *
a * a * a, 
          a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b
* b * a * a * b * b * b * a * a * a * a * a * b * b * b * a * a * b * b * b *
a * a * a, 
          a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b
* b * b * a * a * b * b * b * a * a * a * a * a * b * b * b * a * a * b * b *
b * a * a, 
          a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a
* b * b * b * a * a * b * b * b * a * a * a * a * a * b * b * b * a * a * b *
b * b * a, 
          a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a
* a * b * b * b * a * a * b * b * b * a * a * a * a * a * b * b * b * a * a *
b * b * b, 
          a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a
* a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a *
b * b * b, 
          a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a
* a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 *
b^-1 * b^-1 * b^-1, 
          a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a
* b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b *
b * b * a, 
          a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a
* b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 *
b^-1 * b^-1 * a, 
          a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1
* b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b *
b * a * a, 
          a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b
* b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 *
b^-1 * a * a, 
          a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1
* b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a *
b * b * b, 
          a * a * b * b * b * a * a * a * a * a * b * b * b * a * a * b * b
* b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a *
b * b * b, 
          a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1
* b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b *
a * a * a, 
          a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b * b
* b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 *
a * a * a, 
          a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1
* a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b *
b * b * a, 
          a * b * b * b * a * a * a * a * a * b * b * b * a * a * b * b * b
* a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b *
b * b * a, 
          a * b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1
* b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a *
a * a * a, 
          a * b * b * b * a * a * b * b * b * a * a * a * a * a * b * b * b
* a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a *
a * a * a, 
          b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a
* a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b * b * b * a *
a * b * b, 
          b * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a
* a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a *
a * b * b, 
          b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1
* b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a *
a * b * b, 
          b * a * a * b * b * b * a * a * a * a * a * b * b * b * a * a * b
* b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a *
a * b * b, 
          b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a
* a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b * b * b *
a * a * b, 
          b * b * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a
* a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b *
a * a * b, 
          b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1
* b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a * a *
a * a * b, 
          b * b * a * a * b * b * b * a * a * a * a * a * b * b * b * a * a
* b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a *
a * a * b, 
          b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a
* a * a * a * a * b * b * b * a * a * b * b * b * a * a * a * a * a * b * b *
b * a * a, 
          b * b * b * a * a * a * a * a * b * b * b * a * a * b * b * b * a
* a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b *
b * a * a, 
          b * b * b * a * a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1
* b^-1 * b^-1 * b^-1 * b^-1 * a * a * a * a * a * b * b * b * a * a * b * b * b * a * a *
a * a * a, 
          b * b * b * a * a * b * b * b * a * a * a * a * a * b * b * b * a
* a * b * b * b * a * a * a * a * a * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * b^-1 * a * a *
a * a * a
         ];
G := F / R;

json := Benchmark("GAP", LowIndexSubgroups, [G, 9], 10);
fname := StringFormatted("json/{}-{}-{}-{}-fundamental-o9-154405-9.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
