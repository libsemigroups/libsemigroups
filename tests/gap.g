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

EqualWordGraph := function(D1, D2)
  return OutNeighbours(D1) = OutNeighbours(D2);
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
        Add(que, child);
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
  return OnDigraphs(D, PermList(perm));
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
  local Ds, size, enum, S, subset, T, D;
  Ds := [];
  for size in [1 .. max_size] do
    enum := EnumeratorOfSmallSemigroups(size, 
                                        IsMonoidAsSemigroup, 
                                        true,
                                        x -> Size(MinimalGeneratingSet(x)) <= num_gens, 
                                        true);
    for S in enum do
      S := AsMonoid(IsTransformationMonoid, S);
      for subset in EnumeratorOfTuples(S, num_gens) do
        subset := ShallowCopy(subset);
        Add(subset, One(S));
        T := Semigroup(subset);
        if Size(T) = size then
          D := RightCayleyDigraph(T);
          D := StandardizeWordGraph(D, PositionCanonical(T, One(T)));
          if not ForAny(Ds, x -> EqualWordGraph(D, x)) then
            Add(Ds, D);
          fi;
        fi;
      od;
    od;
  od;
  return Ds;
end;


WordGraphOfAction := function(S)
  Assert(1, IsTransformationMonoid(S));
end;

IsConjugateTransfMonoid := function(S, T)
  local n;
  n := DegreeOfTransformationSemigroup(S);
  return S in Orbit(SymmetricGroup(n), T, POW);
end;

:= function(max_size)
local count, i, j, S, D, Ds, o, Ss; 
  count := 0;
  Ds := []; 
  Ss := [];
  for i in [1 .. (max_size ^ max_size)] do
  for j in [1 .. (max_size ^ max_size)] do
    S := Monoid(TransformationNumber(i, max_size),
                TransformationNumber(j, max_size));
    if Size(S) = max_size  then
      D := DigraphOfActionOnPoints(S, max_size);
      if Size(VerticesReachableFrom(D, 1)) = max_size then
        o := Orbit(SymmetricGroup(max_size), GeneratorsOfSemigroup(S), POW);
        if not ForAny(Ss, x -> x in o) then
          Add(Ds, D);
          Add(Ss, GeneratorsOfSemigroup(S));
          count := count + 1;
        fi;
      fi;
    fi;
  od;
od;
return Ds;

end;
