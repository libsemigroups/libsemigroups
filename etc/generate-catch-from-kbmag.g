# The input to this function is a KBMAG/MAF input record, such as those in the
# files kbmag/standalone/kb_data, and the output is a libsemigroups
# Knuth--Bendix test case using the reduction order specified in the record.
# From the command line:
#
# gap etc/generate-catch-from-kbmag.g input1 input2 input3
#
# The generated test numbers start at 1 and follow the order of the input
# files.

output_alphabet := List([97 .. 122], CharInt);
Append(output_alphabet, List([65 .. 90], CharInt));
Apply(output_alphabet, x -> [x]);

# E, X, and Z are already bound by GAP. Use safe names for those generators
# while evaluating the input, but retain the original names in generated C++.
alphabet := ShallowCopy(output_alphabet);
alphabet[Position(alphabet, "E")] := "kbmag_gen_E";
alphabet[Position(alphabet, "X")] := "kbmag_gen_X";
alphabet[Position(alphabet, "Z")] := "kbmag_gen_Z";

free := FreeGroup(alphabet);

AssignKBMAGGeneratorVariables := function()
  local global_level, warning_level;

  global_level := InfoLevel(InfoGlobal);
  warning_level := InfoLevel(InfoWarning);
  SetInfoLevel(InfoGlobal, 0);
  SetInfoLevel(InfoWarning, 0);
  AssignGeneratorVariables(free);
  SetInfoLevel(InfoGlobal, global_level);
  SetInfoLevel(InfoWarning, warning_level);
end;
AssignKBMAGGeneratorVariables();
IdWord := One(free);

ResetIdentityWord := function()
  IdWord := One(free);
end;

WordToLibsemigroups := function(word)
  local out, index;

  out := "";
  for index in LetterRepAssocWord(word) do
    if index > 0 then
      Append(out, output_alphabet[index]);
    else
      index := AbsInt(index);
      if IsUpperAlphaChar(output_alphabet[index][1]) then
        Append(out, LowercaseString(output_alphabet[index]));
      else
        Append(out, UppercaseString(output_alphabet[index]));
      fi;
    fi;
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

IsKBMAGIdentifierChar := function(char)
  return char in
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
end;

IsKBMAGIdentifierStart := function(char)
  return char in
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
end;

GeneratorOrderIdentifiers := function(contents)
  local char, close_pos, depth, identifiers, open_pos, pos, token;

  pos := PositionSublist(contents, "generatorOrder");
  if pos = fail then
    ErrorNoReturn("the rewriting-system record contains no generatorOrder");
  fi;
  open_pos := pos;
  while open_pos <= Length(contents) and contents[open_pos] <> '[' do
    open_pos := open_pos + 1;
  od;
  if open_pos > Length(contents) then
    ErrorNoReturn("invalid generatorOrder field");
  fi;

  depth := 1;
  close_pos := open_pos + 1;
  while close_pos <= Length(contents) and depth > 0 do
    if contents[close_pos] = '[' then
      depth := depth + 1;
    elif contents[close_pos] = ']' then
      depth := depth - 1;
    fi;
    close_pos := close_pos + 1;
  od;
  if depth <> 0 then
    ErrorNoReturn("unterminated generatorOrder field");
  fi;

  identifiers := [];
  pos := open_pos + 1;
  while pos < close_pos - 1 do
    if IsKBMAGIdentifierStart(contents[pos]) then
      token := "";
      while pos < close_pos - 1
            and IsKBMAGIdentifierChar(contents[pos]) do
        Add(token, contents[pos]);
        pos := pos + 1;
      od;
      if not token in identifiers then
        Add(identifiers, token);
      fi;
    else
      pos := pos + 1;
    fi;
  od;
  return identifiers;
end;

TwoLetterGeneratorReplacements := function(contents)
  local available, candidate, counterpart, identifiers, mapping, name,
  replacement, reserved;

  identifiers := GeneratorOrderIdentifiers(contents);
  reserved := Filtered(identifiers, x -> Length(x) = 1);
  available := Filtered(output_alphabet, x -> not x in reserved);
  mapping := [];

  for name in identifiers do
    if Length(name) = 2 and First(mapping, x -> x[1] = name) = fail then
      counterpart := fail;
      if IsLowerAlphaChar(name[1]) then
        counterpart := Concatenation(UppercaseString(name{[1]}), name{[2]});
      elif IsUpperAlphaChar(name[1]) then
        counterpart := Concatenation(LowercaseString(name{[1]}), name{[2]});
      fi;

      replacement := fail;
      if counterpart <> fail and counterpart in identifiers then
        candidate := First(
          available,
          x -> IsLowerAlphaChar(x[1])
               and UppercaseString(x) in available);
        if candidate <> fail then
          if IsUpperAlphaChar(name[1]) then
            replacement := UppercaseString(candidate);
            Add(mapping, [counterpart, candidate]);
          else
            replacement := candidate;
            Add(mapping, [counterpart, UppercaseString(candidate)]);
          fi;
          Remove(available, Position(available, candidate));
          Remove(available, Position(available, UppercaseString(candidate)));
        fi;
      fi;

      if replacement = fail then
        if IsEmpty(available) then
          ErrorNoReturn("too many generators to map to single letters");
        fi;
        replacement := available[1];
        Remove(available, Position(available, replacement));
      fi;
      Add(mapping, [name, replacement]);
    fi;
  od;
  return mapping;
end;

ReplaceTwoLetterGeneratorNames := function(contents, replacements)
  local index, out, replacement, token;

  out := "";
  index := 1;
  while index <= Length(contents) do
    if IsKBMAGIdentifierChar(contents[index]) then
      token := "";
      while index <= Length(contents)
            and IsKBMAGIdentifierChar(contents[index]) do
        Add(token, contents[index]);
        index := index + 1;
      od;
      replacement := First(replacements, x -> x[1] = token);
      if replacement = fail then
        Append(out, token);
      else
        Append(out, replacement[2]);
      fi;
    else
      Add(out, contents[index]);
      index := index + 1;
    fi;
  od;
  return out;
end;

generator_name_replacements := [];

ReplaceSpecialGeneratorNames := function(contents)
  local char, index, out, replacement;

  out := "";
  for index in [1 .. Length(contents)] do
    char := contents[index];
    replacement := fail;
    if (char = 'E' or char = 'X' or char = 'Z')
       and (index = 1 or not IsKBMAGIdentifierChar(contents[index - 1]))
       and (index = Length(contents)
            or not IsKBMAGIdentifierChar(contents[index + 1])) then
      if char = 'E' then
        replacement := "kbmag_gen_E";
      elif char = 'X' then
        replacement := "kbmag_gen_X";
      else
        replacement := "kbmag_gen_Z";
      fi;
    fi;

    if replacement = fail then
      Add(out, char);
    else
      Append(out, replacement);
    fi;
  od;
  return out;
end;

StripKBMAGComments := function(lines)
  local comment_pos, line, out;

  out := [];
  for line in lines do
    comment_pos := Position(line, '#');
    if comment_pos = fail then
      Add(out, line);
    elif comment_pos > 1 then
      Add(out, line{[1 .. comment_pos - 1]});
    fi;
  od;
  return out;
end;

KBMAGSubgroupFiles := function(name)
  local basename, dirname, entries, parts, prefix;

  parts := SplitString(name, "/");
  basename := parts[Length(parts)];
  if Length(parts) = 1 then
    dirname := ".";
  else
    dirname := JoinStringsWithSeparator(parts{[1 .. Length(parts) - 1]}, "/");
  fi;

  prefix := Concatenation(basename, ".sub");
  entries := Filtered(DirectoryContents(dirname),
                      x -> StartsWith(x, prefix));
  Sort(entries);
  return List(entries, x -> Concatenation(dirname, "/", x));
end;

ReadKBMAGSubgroup := function(name)
  local close_pos, contents, depth, field, kind, open_pos, pos, rws_sub,
  split_name;

  contents := StringFile(name);
  if contents = fail then
    ErrorNoReturn("cannot read the subgroup file ", name);
  fi;
  contents := StripKBMAGComments(SplitString(contents, "\n"));
  contents := JoinStringsWithSeparator(contents, "");
  field := "subGenerators";
  pos := PositionSublist(contents, field);
  if pos = fail then
    field := "normalSubGenerators";
    pos := PositionSublist(contents, field);
  fi;
  if pos = fail then
    ErrorNoReturn("the subgroup file ", name, " contains neither a ",
                  "subGenerators nor a normalSubGenerators field");
  fi;
  if field = "normalSubGenerators" then
    kind := "twosided";
  else
    kind := "onesided";
  fi;

  open_pos := pos;
  while open_pos <= Length(contents) and contents[open_pos] <> '[' do
    open_pos := open_pos + 1;
  od;
  if open_pos > Length(contents) then
    ErrorNoReturn("invalid subGenerators field in ", name);
  fi;

  depth := 1;
  close_pos := open_pos + 1;
  while close_pos <= Length(contents) and depth > 0 do
    if contents[close_pos] = '[' then
      depth := depth + 1;
    elif contents[close_pos] = ']' then
      depth := depth - 1;
    fi;
    close_pos := close_pos + 1;
  od;
  if depth <> 0 then
    ErrorNoReturn("unterminated subGenerators field in ", name);
  fi;

  contents := Concatenation(
    "rec(subGenerators := ",
    contents{[open_pos .. close_pos - 1]},
    ")");
  contents := ReplaceTwoLetterGeneratorNames(
    contents, generator_name_replacements);
  contents := ReplaceSpecialGeneratorNames(contents);

  ResetIdentityWord();
  rws_sub := EvalString(contents);
  if not IsRecord(rws_sub) or not IsBound(rws_sub!.subGenerators) then
    ErrorNoReturn("the subgroup file ", name,
                  " contains no subGenerators field");
  fi;

  split_name := SplitString(name, "/");
  return rec(name := split_name[Length(split_name)],
             generators := rws_sub!.subGenerators,
             kind := kind);
end;

AppendKBInit := function(out, indentation, kind, order_argument)
  Append(out, indentation);
  Append(out, "kb.init(congruence_kind::");
  Append(out, kind);
  Append(out, ", p");
  if order_argument <> fail then
    Append(out, ", std::vector<size_t>(");
    Append(out, order_argument);
    Append(out, ")");
  fi;
  Append(out, ");\n");
end;

RWSToLibsemigroups := function(rws, nr, comment, test_case_label, subgroups)
  local test_number, prefix, gens, order_argument, comparator, out, x,
  index, rule, subgroup, word, section_name, file_name;

  if not IsRecord(rws) then
    ErrorNoReturn("usage: the first argument <rws> must be a record,");
  elif not IsPosInt(nr) and not IsString(nr) then
    ErrorNoReturn("usage: the second argument <nr> must be a pos. int. or ",
                  "a string,");
  elif not IsString(comment) then
    ErrorNoReturn("usage: the third argument <comment> must be a string,");
  elif not IsList(subgroups) then
    ErrorNoReturn("usage: the fifth argument <subgroups> must be a list,");
  fi;

  if IsString(nr) then
    test_number := nr;
  else
    test_number := String(nr);
    prefix := ListWithIdenticalEntries(3 - Length(test_number), '0');
    test_number := Concatenation(prefix, test_number);
  fi;

  gens := "";
  for x in rws!.generatorOrder do
    Append(gens, WordToLibsemigroups(x));
  od;

  order_argument := fail;
  if not IsBound(rws!.ordering) then
    Print("// No reduction ordering specified for ", test_case_label, " skipping !!!\n\n");
    return "";
  fi;
  if rws!.ordering = "shortlex" then
    comparator := "LenLexCmp";
  elif rws!.ordering = "short_rtlex" then
    comparator := "RevLenLexCmp";
  elif rws!.ordering = "recursive" then
    comparator := "RevRPOCmp";
  elif rws!.ordering = "rt_recursive" then
    comparator := "RPOCmp";
  elif rws!.ordering = "wreathprod" then
    if not IsBound(rws!.level) then
      ErrorNoReturn("a wreathprod ordering requires a level field");
    elif Length(rws!.level) <> Length(rws!.generatorOrder) then
      ErrorNoReturn("the level and generatorOrder fields must have equal ",
                    "length");
    fi;
    comparator := "WreathCmp";
    order_argument := SizeTVectorToLibsemigroups(rws!.level);
  elif rws!.ordering = "rt_wreathprod" then
    if not IsBound(rws!.level) then
      ErrorNoReturn("a wreathprod ordering requires a level field");
    elif Length(rws!.level) <> Length(rws!.generatorOrder) then
      ErrorNoReturn("the level and generatorOrder fields must have equal ",
                    "length");
    fi;
    comparator := "RevWreathCmp";
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
    Print("// Unsupported reduction ordering ", rws!.ordering, " in ",
    test_case_label, " skipping !!!\n\n");
    return "";
  fi;
  file_name := Last(SplitString(First(SplitString(test_case_label, "/")), " "));

  out := Concatenation(comment, "\n");
  Append(out, "LIBSEMIGROUPS_TEST_CASE(\"KnuthBendix\",\n");
  Append(out, "                        \"");
  Append(out, test_number);
  Append(out, "\",\n                        \"");
  Append(out, test_case_label);
  Append(out, "\",\n                        \"[quick][maf][");
  Append(out, file_name);
  Append(out, "]\"");
  Append(out, ") {\n");
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
        Append(out, WordToLibsemigroups(rws!.generatorOrder[index]));
        Append(out, WordToLibsemigroups(rws!.inverses[index]));
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
  Append(out, "  KnuthBendix<std::string, RewritingSystem> kb;\n");

  if IsEmpty(subgroups) then
    AppendKBInit(out, "  ", "twosided", order_argument);
    Append(out, "  kb.run();\n");
    Append(out,
           "  REQUIRE(kb.rewriting_system().number_of_rules() == 0);\n");
    Append(out, "  REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);\n");
  else
    Append(out, "\n  SECTION(\"");
    section_name := SplitString(test_case_label, "/");
    Append(out, Last(section_name));
    Append(out, "\") {\n");
    AppendKBInit(out, "    ", "twosided", order_argument);
    Append(out, "    kb.run();\n");
    Append(out,
           "    REQUIRE(kb.rewriting_system().number_of_rules() == 0);\n");
    Append(out,
           "    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);\n");
    Append(out, "  }\n");

    for subgroup in subgroups do
      Append(out, "\n  SECTION(\"");
      Append(out, subgroup!.name);
      Append(out, "\") {\n");
      AppendKBInit(out, "    ", subgroup!.kind, order_argument);
      for word in subgroup!.generators do
        Append(out, "    knuth_bendix::add_generating_pair(kb, \"");
        Append(out, WordToLibsemigroups(word));
        Append(out, "\", \"\");\n");
      od;
      Append(out, "\n    kb.run();\n");
      Append(out,
             "    REQUIRE(kb.rewriting_system().number_of_rules() == 0);\n");
      Append(out,
             "    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);\n");
      Append(out, "  }\n");
    od;
  fi;
  Append(out, "}\n\n");
  return out;
end;

KBMAGFileToLibsemigroups := function(name, nr)
  local contents, start_pos, comment, test_case_label, end_pos, pos,
  original_name, subgroup_files, subgroups;

  original_name := name;
  contents := StringFile(name);
  if contents = fail then
    ErrorNoReturn("cannot read the file ", name);
  fi;
  contents := SplitString(contents, "\n");

  start_pos := PositionProperty(contents, x -> not IsEmpty(x) and x[1] <> '#');
  if start_pos = fail then
    ErrorNoReturn("the file ", name, " contains no rewriting-system record");
  elif start_pos <> 1 then
    comment := contents{[1 .. start_pos - 1]};
    Apply(comment, function(x)
      if Length(x) < 2 then
        return "";
      fi;
      return Concatenation("// ", x{[2 .. Length(x)]});
    end);
    Perform(comment, NormalizeWhitespace);
    comment := JoinStringsWithSeparator(comment, "\n");
    Append(comment, " ");
  else
    comment := "";
  fi;
  test_case_label := "MAF: ";

  end_pos := PositionProperty(contents, x -> StartsWith(x, ");"));

  # Remove initial comments and the "_RWS := " assignment.
  contents := contents{[start_pos .. end_pos]};
  contents := StripKBMAGComments(contents);
  contents := JoinStringsWithSeparator(contents, "");

  pos := PositionSublist(contents, "rec");

  contents := contents{[pos .. Length(contents)]};
  generator_name_replacements := TwoLetterGeneratorReplacements(contents);
  contents := ReplaceTwoLetterGeneratorNames(
    contents, generator_name_replacements);
  contents := ReplaceSpecialGeneratorNames(contents);

  name := SplitString(name, "/");
  pos := Position(name, "kbmag");
  if pos <> fail then
    name := name{[pos .. Length(name)]};
  fi;
  pos := Position(name, "examples");
  if pos <> fail then
    name := name{[pos + 1 .. Length(name)]};
  fi;
  pos := Position(name, "subgroups");
  if pos <> fail then
    name := name{[pos .. Length(name)]};
  fi;

  name := JoinStringsWithSeparator(name, "/");
  Append(test_case_label, name);

  subgroup_files := KBMAGSubgroupFiles(original_name);
  subgroups := List(subgroup_files, ReadKBMAGSubgroup);

  ResetIdentityWord();
  return RWSToLibsemigroups(EvalString(contents), nr, comment, test_case_label,
                            subgroups);
end;

PrintKBMAGFileToLibsemigroups := function(name, nr)
  Print(KBMAGFileToLibsemigroups(name, nr));
end;

RunKBMAGFileToLibsemigroups := function()
  local args, input_file, nr, script_pos;

  args := GAPInfo.SystemCommandLine;
  script_pos := PositionProperty(
    args,
    x -> IsString(x)
         and PositionSublist(x, "generate-catch-from-kbmag.g") <> fail);

  # If this file was loaded using Read(), retain the interactive API above.
  if script_pos = fail then
    return;
  elif script_pos = Length(args) then
    Print("usage: gap etc/generate-catch-from-kbmag.g ",
          "input.file [input.file ...]\n");
    QUIT_GAP(1);
  fi;

  nr := 1;
  for input_file in args{[script_pos + 1 .. Length(args)]} do
    PrintKBMAGFileToLibsemigroups(input_file, nr);
    nr := nr + 1;
  od;
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
