# The input to this function is a KBMAG/MAF input record, such as those in the
# files kbmag/standalone/kb_data, and the output is a libsemigroups
# Knuth--Bendix test case using the reduction order specified in the record.
# From the command line:
#
# gap etc/test-kbmag.g kbmag/standalone/kb_data/example
#
# An optional third argument specifies the test number:
#
# gap etc/test-kbmag.g kbmag/standalone/kb_data/example 158

alphabet := List([97 .. 122], CharInt);
Append(alphabet, List([65 .. 90], CharInt));
Apply(alphabet, x -> [x]);
Remove(alphabet, Position(alphabet, "E"));
Remove(alphabet, Position(alphabet, "X"));
Remove(alphabet, Position(alphabet, "Z"));

free := FreeMonoid(alphabet);
AssignGeneratorVariables(free);
IdWord := One(free);

ResetIdentityWord := function()
  IdWord := One(free);
end;

WordToLibsemigroups := function(word)
  local out, index;

  out := "";
  for index in LetterRepAssocWord(word) do
    Append(out, alphabet[index]);
  od;
  return out;
end;

SizeTVectorToLibsemigroups := function(values)
  local out, separator, value;

  out := "{";
  separator := "";
  for value in values do
    Append(out, separator);
    Append(out, String(value));
    separator := ", ";
  od;
  Append(out, "}");
  return out;
end;

RWSToLibsemigroups := function(rws, nr, comment, test_case_label)
  local comparator, gens, index, order_argument, out, rule, test_number, x;

  if not IsRecord(rws) then
    ErrorNoReturn("usage: the first argument <rws> must be a record,");
  elif not IsPosInt(nr) and not IsString(nr) then
    ErrorNoReturn("usage: the second argument <nr> must be a pos. int. or ",
                  "a string,");
  elif not IsString(comment) then
    ErrorNoReturn("usage: the third argument <comment> must be a string,");
  fi;

  if IsString(nr) then
    test_number := nr;
  else
    test_number := String(nr);
  fi;

  gens := "";
  for x in rws!.generatorOrder do
    Append(gens, alphabet[LetterRepAssocWord(x)[1]]);
  od;

  order_argument := fail;
  if rws!.ordering = "shortlex" then
    comparator := "LenLexCmp";
  elif rws!.ordering = "recursive" then
    comparator := "RevRPOCmp";
  elif rws!.ordering = "wreathprod" then
    if not IsBound(rws!.level) then
      ErrorNoReturn("a wreathprod ordering requires a level field");
    elif Length(rws!.level) <> Length(rws!.generatorOrder) then
      ErrorNoReturn("the level and generatorOrder fields must have equal ",
                    "length");
    fi;
    comparator := "WreathCmp";
    order_argument := SizeTVectorToLibsemigroups(rws!.level);
  elif rws!.ordering = "wtlex" then
    if not IsBound(rws!.weight) then
      ErrorNoReturn("a wtlex ordering requires a weight field");
    elif Length(rws!.weight) <> Length(rws!.generatorOrder) then
      ErrorNoReturn("the weight and generatorOrder fields must have equal ",
                    "length");
    fi;
    comparator := "WtLexCmp";
    order_argument := SizeTVectorToLibsemigroups(rws!.weight);
  elif rws!.ordering = "wtshortlex" then
    if not IsBound(rws!.weight) then
      ErrorNoReturn("a wtshortlex ordering requires a weight field");
    elif Length(rws!.weight) <> Length(rws!.generatorOrder) then
      ErrorNoReturn("the weight and generatorOrder fields must have equal ",
                    "length");
    fi;
    comparator := "WtLenLexCmp";
    order_argument := SizeTVectorToLibsemigroups(rws!.weight);
  else
    ErrorNoReturn("unsupported reduction ordering ", rws!.ordering);
  fi;

  out := Concatenation("//  ", comment, "\n");
  Append(out, "LIBSEMIGROUPS_TEST_CASE(\"KnuthBendix\",\n");
  Append(out, "                        \"");
  Append(out, test_number);
  Append(out, "\",\n                        \"");
  Append(out, test_case_label);
  Append(out, "\",\n                        \"[knuth-bendix][kbmag][");
  Append(out, rws!.ordering);
  Append(out, "]\") {\n");
  Append(out, "  auto rg = ReportGuard(false);\n\n");
  Append(out, "  Presentation<std::string> p;\n");
  Append(out, "  p.alphabet(\"");
  Append(out, gens);
  Append(out, "\").contains_empty_word(true);\n\n");

  # The inverses field is part of the presentation described by a KBMAG/MAF
  # record. A hole in the list means that no inverse was specified.
  if IsBound(rws!.inverses) then
    for index in [1 .. Length(rws!.generatorOrder)] do
      if IsBound(rws!.inverses[index]) then
        Append(out, "  presentation::add_rule(p, \"");
        Append(out,
               WordToLibsemigroups(rws!.generatorOrder[index]
                                   * rws!.inverses[index]));
        Append(out, "\", \"\");\n");
      fi;
    od;
  fi;

  for rule in rws!.equations do
    Append(out, "  presentation::add_rule(p, \"");
    Append(out, WordToLibsemigroups(rule[1]));
    Append(out, "\", \"");
    Append(out, WordToLibsemigroups(rule[2]));
    Append(out, "\");\n");
  od;

  Append(out, "\n  using RewritingSystem =\n");
  Append(out, "      detail::RewritingSystemTrie<");
  Append(out, comparator);
  Append(out, ">;\n");
  Append(out, "  KnuthBendix<std::string, RewritingSystem> kb(\n");
  Append(out, "      congruence_kind::twosided, p");
  if order_argument <> fail then
    Append(out, ", std::vector<size_t>(");
    Append(out, order_argument);
    Append(out, ")");
  fi;
  Append(out, ");\n");
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

  pos := PositionProperty(contents, x -> not IsEmpty(x) and x[1] <> '#');
  if pos = fail then
    ErrorNoReturn("the file ", name, " contains no rewriting-system record");
  elif pos <> 1 then
    comment := contents{[1 .. pos - 1]};
    Apply(comment, function(x)
      if Length(x) < 2 then
        return "";
      fi;
      return x{[2 .. Length(x)]};
    end);
    comment := Concatenation(comment);
    Append(comment, " ");
  else
    comment := "";
  fi;
  test_case_label := "(from ";

  # Remove initial comments and the "_RWS := " assignment.
  contents := contents{[pos .. Length(contents)]};
  contents := JoinStringsWithSeparator(contents, "");
  contents := contents{[9 .. Length(contents)]};

  name := SplitString(name, "/");
  pos := Position(name, "kbmag");
  if pos <> fail then
    name := name{[pos .. Length(name)]};
  fi;
  name := JoinStringsWithSeparator(name, "/");
  Append(test_case_label, name);
  Append(test_case_label, ")");

  ResetIdentityWord();
  return RWSToLibsemigroups(EvalString(contents), nr, comment, test_case_label);
end;

PrintKBMAGFileToLibsemigroups := function(name, nr)
  Print(KBMAGFileToLibsemigroups(name, nr));
end;

RunKBMAGFileToLibsemigroups := function()
  local args, input_file, nr, script_pos;

  args := GAPInfo.SystemCommandLine;
  script_pos := PositionProperty(
    args,
    x -> IsString(x) and PositionSublist(x, "test-kbmag.g") <> fail);

  # If this file was loaded using Read(), retain the interactive API above.
  if script_pos = fail then
    return;
  elif script_pos = Length(args) then
    Print("usage: gap etc/test-kbmag.g input.file [test-number]\n");
    QUIT_GAP(1);
  fi;

  input_file := args[script_pos + 1];
  nr := "000";
  if script_pos + 1 < Length(args) then
    nr := args[script_pos + 2];
    if IsEmpty(nr) or not ForAll(nr, x -> x in "0123456789") then
      Print("error: test-number must contain only decimal digits\n");
      QUIT_GAP(1);
    fi;
  fi;

  PrintKBMAGFileToLibsemigroups(input_file, nr);
  QUIT_GAP(0);
end;

KBMAGToLibsemigroupsDirectory := function(dirname, outfile)
  local nr, out, filename;

  nr := 43;
  out := "";
  for filename in DirectoryContents(dirname) do
    if not '.' in filename and filename <> "a4"
       and not filename in ["degen4c", "s3"] then
      filename := Concatenation(dirname, filename);
      Print("Reading file ", filename, " . . . \n");
      Append(out, KBMAGFileToLibsemigroups(filename, nr));
      nr := nr + 1;
    fi;
  od;
  FileString(outfile, out);
end;

RunKBMAGFileToLibsemigroups();
