"""This file contains functions that can be used to process dataframes
containing test timing data."""


def join_means(base, *others, names):
    """Take a dataframe <base>, and a list of dataframes <others>, and return a
    single dataframe containing the mean time and standard deviation for all the
    tests run in <base>, and any corresponding times from dataframes in
    <others>. The names of the time columns is specified in <names>. The column
    containing the standard deviation info is called "stddev".
    """
    assert len(others) + 1 == len(names)

    base.rename(columns={"mean": names[0]}, inplace=True)
    for i, other in enumerate(others):
        other.rename(columns={"mean": names[i + 1]}, inplace=True)
    return base[[names[0], "stddev"]].join(
        (other[names[i + 1]] for i, other in enumerate(others))
    )


def significantly_different_times(df, factor):
    """Given a dataframe with records of the form
    (base, stddev, comp_1, comp_2, ...), return a dataframe with records that
    satisfy:
    comp_i < base - factor * stddev or comp_i > base + factor * stddev
    for some i.

    In other words, return a dataframe whose records consist of those tests
    whose times are significantly different from the base.
    """
    base_col = df.columns[0]
    stddev_col = df.columns[1]
    comparison_cols = df.columns[2:]
    conditional = (
        df[comparison_cols].values
        < df[[base_col]].values - factor * df[[stddev_col]].values
    ).any(axis=1) | (
        df[comparison_cols].values
        > df[[base_col]].values + factor * df[[stddev_col]].values
    ).any(axis=1)

    return df[conditional]
