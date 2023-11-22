LoadPackage("Digraphs");
LoadPackage("Semigroups");

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

StandardizeWordGraph := function(D, root)
  local i, alph, letter, que, seen, node, child, next_node, perm;

  # TODO arg checks
  if IsNullDigraph(D) then
    return D;
  fi;

  i := 1;
  que := [root];
  seen := BlistList([1 .. DigraphNrVertices(D)], [root]);
  next_node := OutNeighbours(D);
  alph := Length(next_node[root]);

  while i <= Length(que) do
    node := que[i];
    for letter in [1 .. alph] do
      child := next_node[node][letter];
      if not seen[child] then
        Append(que, child);
        seen[child] := true;
      fi;
    od;
    i := i + 1;
  od;

  if Length(que) <> DigraphNrVertices(D) then
    # Not all nodes reachable from root!
    return fail;
  fi;
  
  perm := List([1 .. Length(que)], x -> 0);
  for i in [1 .. Length(que)] do
    perm[que[i]] := i;
  od;
  return OnDigraphs(D, perm);
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
