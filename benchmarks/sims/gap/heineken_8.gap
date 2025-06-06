# gap -A --nointeract heineken_8.gap

Read("benchmark.g");
F := FreeGroup("a", "c", "e");
AssignGeneratorVariables(F);;
R := [
            a^-1 * c^-1 * a^-1 * c * a * c^-1 * a * c * e^-1, 
            a^-1 * c^-1 * e^-1 * c^-1 * e * c * e^-1 * c * e, 
            a^-1 * c^-1 * a * c * a * e * c^-1 * a^-1 * c, 
            a^-1 * e^-1 * a * e^-1 * a^-1 * e * a * e * c, 
            a^-1 * e^-1 * a * e * a^-1 * e * a * c^-1 * e^-1, 
            a^-1 * c * a^-1 * c^-1 * a * c * a * e * c^-1, 
            a^-1 * c * a * c^-1 * a * c * e^-1 * a^-1 * c^-1, 
            a^-1 * e * a * c^-1 * e^-1 * a^-1 * e^-1 * a * e, 
            a^-1 * e * a * e * c * a^-1 * e^-1 * a * e^-1, 
            c^-1 * a^-1 * c * a^-1 * c^-1 * a * c * a * e, 
            c^-1 * a^-1 * c * a * c^-1 * a * c * e^-1 * a^-1, 
            c^-1 * e^-1 * a^-1 * e^-1 * a * e * a^-1 * e * a, 
            c^-1 * e^-1 * c^-1 * e * c * e^-1 * c * e * a^-1, 
            c^-1 * e^-1 * c * e * c * a * e^-1 * c^-1 * e, 
            c^-1 * a * c * e^-1 * a^-1 * c^-1 * a^-1 * c * a, 
            c^-1 * a * c * a * e * c^-1 * a^-1 * c * a^-1, 
            c^-1 * e * c^-1 * e^-1 * c * e * c * a * e^-1, 
            c^-1 * e * c * e^-1 * c * e * a^-1 * c^-1 * e^-1, 
            e^-1 * a^-1 * c^-1 * a^-1 * c * a * c^-1 * a * c, 
            e^-1 * a^-1 * e^-1 * a * e * a^-1 * e * a * c^-1, 
            e^-1 * a^-1 * e * a * e * c * a^-1 * e^-1 * a, 
            e^-1 * c^-1 * e * c^-1 * e^-1 * c * e * c * a, 
            e^-1 * c^-1 * e * c * e^-1 * c * e * a^-1 * c^-1, 
            e^-1 * a * e^-1 * a^-1 * e * a * e * c * a^-1, 
            e^-1 * a * e * a^-1 * e * a * c^-1 * e^-1 * a^-1, 
            e^-1 * c * e * a^-1 * c^-1 * e^-1 * c^-1 * e * c, 
            e^-1 * c * e * c * a * e^-1 * c^-1 * e * c^-1, 
            a * c^-1 * e^-1 * a^-1 * e^-1 * a * e * a^-1 * e, 
            a * c^-1 * a * c * e^-1 * a^-1 * c^-1 * a^-1 * c, 
            a * e^-1 * a^-1 * e * a * e * c * a^-1 * e^-1, 
            a * e^-1 * c^-1 * e * c^-1 * e^-1 * c * e * c, 
            a * c * e^-1 * a^-1 * c^-1 * a^-1 * c * a * c^-1, 
            a * c * a * e * c^-1 * a^-1 * c * a^-1 * c^-1, 
            a * e * a^-1 * e * a * c^-1 * e^-1 * a^-1 * e^-1, 
            a * e * c^-1 * a^-1 * c * a^-1 * c^-1 * a * c, 
            a * e * c * a^-1 * e^-1 * a * e^-1 * a^-1 * e, 
            c * a^-1 * c^-1 * a * c * a * e * c^-1 * a^-1, 
            c * a^-1 * e^-1 * a * e^-1 * a^-1 * e * a * e, 
            c * e^-1 * a^-1 * c^-1 * a^-1 * c * a * c^-1 * a, 
            c * e^-1 * c * e * a^-1 * c^-1 * e^-1 * c^-1 * e, 
            c * a * c^-1 * a * c * e^-1 * a^-1 * c^-1 * a^-1, 
            c * a * e^-1 * c^-1 * e * c^-1 * e^-1 * c * e, 
            c * a * e * c^-1 * a^-1 * c * a^-1 * c^-1 * a, 
            c * e * a^-1 * c^-1 * e^-1 * c^-1 * e * c * e^-1, 
            c * e * c * a * e^-1 * c^-1 * e * c^-1 * e^-1, 
            e * a^-1 * c^-1 * e^-1 * c^-1 * e * c * e^-1 * c, 
            e * a^-1 * e * a * c^-1 * e^-1 * a^-1 * e^-1 * a, 
            e * c^-1 * a^-1 * c * a^-1 * c^-1 * a * c * a, 
            e * c^-1 * e^-1 * c * e * c * a * e^-1 * c^-1, 
            e * a * c^-1 * e^-1 * a^-1 * e^-1 * a * e * a^-1, 
            e * a * e * c * a^-1 * e^-1 * a * e^-1 * a^-1, 
            e * c * a^-1 * e^-1 * a * e^-1 * a^-1 * e * a, 
            e * c * e^-1 * c * e * a^-1 * c^-1 * e^-1 * c^-1, 
            e * c * a * e^-1 * c^-1 * e * c^-1 * e^-1 * c, 
           ];
G := F / R;;
json := Benchmark("GAP", LowIndexSubgroups, [G, 8], 10);
fname := StringFormatted("json/{}-{}-{}-{}-heineken-8.gap.json",
                         NowString(),
                         GAPInfo.KernelInfo.uname.machine, 
                         LowercaseString(GAPInfo.KernelInfo.uname.sysname), 
                         GAPInfo.KernelInfo.uname.release); 
PrintFormatted("Writing file {} . . .\n", fname);
FileString(fname, json);
QuitGap();
