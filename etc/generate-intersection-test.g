# This GAP script implements some functions for generating test cases for the 
# intersect function of schreier-sims-helper.hpp

# Taken from the ferret package
RandomGroupOfSize := function(rgen, size)
    return PrimitiveGroup(size, Random(rgen, [1..NrPrimitiveGroups(size)]));
end;;

RandomGroupUpToSize := function(rgen, size)
    local s;
    s := Random([1..size]);
    if s = 1 then
      return Group(());
    else
      return RandomGroupOfSize(rgen, s);
    fi;
end;;

PermutationToListOfImages := function(n, perm)
  return List([1 .. n], x -> (x^perm)-1);
end;;

GeneratePermIntersectionTestCase := function(n, seed)
  local S, T, U, rs1, gensS, gensT, gensU, i;
  S := CyclicGroup(2);
  T := CyclicGroup(2);
  U := TrivialGroup();

  rs1 := RandomSource(IsMersenneTwister, seed);
  while Size(U) = 1 or Size(U) = Size(S) or Size(U) = Size(T) do
    S := RandomGroupOfSize(rs1, n);
    T := RandomGroupOfSize(rs1, n);
    U := Intersection(S, T);
  od;

  gensS := List(GeneratorsOfGroup(S), x->PermutationToListOfImages(n,x));
  for gen in gensS do
    Print("S.add_generator(Perm({");
    for i in [1 .. n] do
      Print(gen[i]);
      if i <> n then
        Print(", ");
        fi;
    od;
    Print("}));\n");
  od;

  gensT := List(GeneratorsOfGroup(T), x->PermutationToListOfImages(n,x));
  for gen in gensT do
    Print("T.add_generator(Perm({");
    for i in [1 .. n] do
      Print(gen[i]);
      if i <> n then
        Print(", ");
        fi;
    od;
    Print("}));\n");
  od;
  Print("schreier_sims_helper::intersection(U, S, T);\n");
  Print("REQUIRE(U.size() == ", Size(U), ");\n");

  gensU := List(GeneratorsOfGroup(U), x->PermutationToListOfImages(n,x));
  for gen in gensU do
    Print("REQUIRE(U.contains(Perm({");
    for i in [1 .. n] do
      Print(gen[i]);
      if i <> n then
        Print(", ");
        fi;
    od;
    Print("})));\n");
  od;
end;
