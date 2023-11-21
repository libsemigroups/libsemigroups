AsWordGraph := function(C)
  local lookup, S, A, out, next, pos, class, a;

  lookup := EquivalenceRelationCanonicalLookup(C);
  S      := Source(C);
  A      := GeneratorsOfSemigroup(S);

  out := [];

  for class in EquivalenceClasses(C) do
    next := [];
    for a in A do
      pos := PositionCanonical(S, Representative(class) * a);
      Add(next, lookup[pos]);
    od;
    Add(out, next);
  od;

  return Digraph(out);
end;

StandardizeWordGraph := function(D)
  local s, t, N, n, result, x, r;

  # TODO arg checks
  if IsNullDigraph(D) then
    return D;
  fi;
  s := 0;
  t := 0;
  N := OutNeighbours(D);
  n := Length(N[1]);
  result := DigraphMutableCopy(D);

  while s <= t do
    for x in [1 .. n] do
      r := N[s + 1][x];
      if r > t then 
        t := t + 1;
        if r > t then
          OnDigraphs(result, (t + 1, r));
          N := OutNeighbours(result);
        fi;
      fi;
    od;
    s := s + 1;
  od;
  return result;
end;

ToWordGraphs := function(Cs)
  local Ds;
  Ds := List(Cs, AsWordGraph);
  Ds := List(Ds, x -> List(x, y -> y{[2 .. Length(y)]}));
  return Ds - 1;
end;

AllMultiplicationTables:= function(S)
  local result;
  result :=  ShallowCopy(Orbit(SymmetricGroup(Size(S)), 
               MultiplicationTable(S),
               OnMultiplicationTable));
  if not IsSelfDualSemigroup(S) then
    Append(result, 
            Orbit(SymmetricGroup(Size(S)), 
            TransposedMat(MultiplicationTable(S)),
            OnMultiplicationTable));
  fi;
  return result;
end;



ExpectedNumber := function(num_gens, max_size)
  
  
end;
