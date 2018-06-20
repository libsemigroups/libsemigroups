# The input to this function is a KBMAG input record, such as those in the
# files kbmag/standalone/kb_data, and the output is a test case as in
# libsemigroups/tests/rws.test.cc

LoadPackage("kbmag", false);

alphabet := List([97 .. 122], CharInt);
Append(alphabet, List([65 .. 90], CharInt));
Apply(alphabet, x -> [x]);
Remove(alphabet, Position(alphabet, "E"));
Remove(alphabet, Position(alphabet, "X"));
Remove(alphabet, Position(alphabet, "Z"));

free := FreeMonoid(alphabet);
AssignGeneratorVariables(free);
IdWord := One(free);

InitThisShit := function()
  IdWord := One(free);
end;

RWSToLibsemigroups := function(rws, nr, comment, test_case_label)
  local tmp, nr_rules, gens, out, x, rule, index;
  
  if not IsRecord(rws) then 
    ErrorNoReturn("usage: the first argument <rws> must be a record,");
  elif not IsPosInt(nr) then 
    ErrorNoReturn("usage: the second argument <nr> must be an pos. int.,");
  elif not IsString(comment) then
    ErrorNoReturn("usage: the third argument <comment> must be a string,");
  fi;

  tmp := KBMAGRewritingSystem(free / rws!.equations);
  KnuthBendix(tmp);
  nr_rules := Size(tmp!.equations);
  Error();
  #nr_rules := 0;
  gens := "";
  for x in rws.generatorOrder do
    Append(gens, alphabet[LetterRepAssocWord(x)[1]]);
  od;
  out := Concatenation("//  ", comment, "\n");
  Append(out, "TEST_CASE(\"RWS ");
  Append(out, String(nr));
  Append(out, ": ");
  Append(out, test_case_label);
  Append(out, "\", \"[rws][kbmag]");
  Append(out, "[");
  Append(out, rws.ordering);
  Append(out, "]");
  Append(out, "[");
  Append(out, String(nr));
  Append(out, "]");
  Append(out, "\") {\n  ");
  Append(out, "RWS rws(\"");
  Append(out, gens);
  Append(out, "\");\n");

  for rule in rws!.equations do 
    Append(out, "  rws.add_rule(");
    Append(out, "\"");
    for index in LetterRepAssocWord(rule[1]) do 
      Append(out, alphabet[index]);
    od;

    Append(out, "\", ");

    Append(out, "\"");
    for index in LetterRepAssocWord(rule[2]) do 
      Append(out, alphabet[index]);
    od;
    Append(out, "\");\n");
  od;
  Append(out, "  rws.set_report(RWS_REPORT);\n\n");

  Append(out, "  REQUIRE(!rws.confluent());\n\n");
  Append(out, "  rws.knuth_bendix();\n");
  Append(out, "  REQUIRE(rws.confluent());\n");
  Append(out, "  REQUIRE(rws.nr_rules() == ");
  Append(out, String(nr_rules));
  Append(out, ");\n\n");
 
  if nr_rules <> 0 and nr_rules < 50 then 
    for rule in tmp!.equations do 
      Append(out, "  REQUIRE(rws.rule(");
      Append(out, "\"");
      for index in rule[1] do 
        Append(out, alphabet[index]);
      od;

      Append(out, "\", ");

      Append(out, "\"");
      for index in rule[2] do 
        Append(out, alphabet[index]);
      od;
      Append(out, "\"));\n");
    od;
  fi;

  Append(out, "}\n\n");
  return out;
end;

KBMAGFileToLibsemigroups := function(name, nr)
  local contents, pos, comment, test_case_label;

  contents := StringFile(name);
  if contents = fail then 
    ErrorNoReturn("cannot read the file ", name);
  fi;
  contents := SplitString(contents, "\n");

  pos := PositionProperty(contents, x -> x[1] <> '#');
  if pos <> 1 then 
    comment := contents{[1 .. pos - 1]};
    Apply(comment, x -> x{[2 .. Length(x)]});
    comment := Concatenation(comment);
    Append(comment, " ");
  else 
    comment := "";
  fi;
  test_case_label := "(from ";

  # Remove the comment in first line
  contents := contents{[pos .. Length(contents)]}; 
  contents := JoinStringsWithSeparator(contents, "");
  # Remove the "_RWS := " from the beginning
  contents := contents{[9 .. Length(contents)]};

  name := SplitString(name, "/");
  pos := Position(name, "kbmag");
  name := name{[pos .. Length(name)]};
  name := JoinStringsWithSeparator(name, "/");
  Append(test_case_label, name);
  Append(test_case_label, ")");

  InitThisShit(); #dunno why this is necessary;
  return RWSToLibsemigroups(EvalString(contents), nr, comment, test_case_label);
  #return EvalString(contents);
end;

KBMAGToLibsemigroupsDirectory := function(dirname, outfile)
  local nr, out, filename;
  nr := 43;
  out := "";
  for filename in DirectoryContents(dirname) do 
    if not '.' in filename and filename <> "a4" and not filename in ["degen4c",
      "s3"] then 
      filename := Concatenation(dirname, filename);
      Print("Reading file ", filename, " . . . \n");
      Append(out, KBMAGFileToLibsemigroups(filename, nr));
      nr := nr + 1;
    fi;
  od;
  FileString(outfile, out);
  return;
end;
