LoadPackage("io");
LoadPackage("json");

__JDMS_GLOBAL_TIMINGS_RECORD := rec(running := false);

StartTimer := function()
  if not __JDMS_GLOBAL_TIMINGS_RECORD.running then
    __JDMS_GLOBAL_TIMINGS_RECORD.timeofday := IO_gettimeofday();
  fi;
end;

# Time in seconds!
ElapsedTimer := function()
  local  timeofday, elapsed;
  if IsBound(__JDMS_GLOBAL_TIMINGS_RECORD) and
      IsBound(__JDMS_GLOBAL_TIMINGS_RECORD.timeofday) then

    timeofday := IO_gettimeofday();
    elapsed := Float((timeofday.tv_sec - __JDMS_GLOBAL_TIMINGS_RECORD.timeofday.tv_sec)
                * 10 ^ 6 + (timeofday.tv_usec -
                  __JDMS_GLOBAL_TIMINGS_RECORD.timeofday.tv_usec)) / 10 ^ 6;
    return elapsed;
  else
    return 0;
  fi;
end;

StopTimer := function()
  local t;
  t := ElapsedTimer();
  __JDMS_GLOBAL_TIMINGS_RECORD.running := false;
  Unbind(__JDMS_GLOBAL_TIMINGS_RECORD.timeofday);
  return t;
end;

Benchmark := function(name, func, arg, trials)
  local result, data, t, mean, i;

  result := rec(benchmarks := [rec(name := name, stats := rec(data:= [], stddev :=[]))]);
  data := result.benchmarks[1].stats.data;

  # Warmup
  if trials >= 100 then
    for i in [1 .. 10] do
      CallFuncList(func, arg);
    od;
  fi;
  for i in [1 .. trials] do
    StartTimer();
    Info(InfoWarning, 1, "Performing trial ", i, " of ", trials);
    CallFuncList(func, arg);
    t := StopTimer();
    Add(data, t);
    if Length(data) = 1 and trials > 1 then
      Info(InfoWarning, 1, "Estimated time is ", data[1] * trials, " seconds");
    fi;
  od;
  GASMAN("collect");
  Info(InfoWarning, 1, "All values in seconds!");
  mean := Float(Sum(data / trials));
  result.benchmarks[1].stats.mean := mean;
  result.benchmarks[1].stats.stddev := Sqrt(Sum(data, x -> Float((x - mean) ^ 2) / trials));
  return GapToJsonString(result);
end;

NowString := function()
  local date, stdin, result, stdout;
  date := PathSystemProgram( "date" );
  stdin := InputTextUser();
  result := ""; 
  stdout := OutputTextString(result, false);
  Process(DirectoryCurrent(), date, stdin, stdout, ["+%Y-%m-%dT%H-%M-%S"]);
  return Chomp(result);
end;

if not IsExistingFile("json") then
  Exec("mkdir json");
fi;
