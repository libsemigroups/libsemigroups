# Deprecation policy

This page explains the deprecation policy for `libsemigroups`, it's not very
complicated. 

Any function, type, or alias with the attribute `[[deprecated]]`
will be removed in the next major release. So, the alias \ref
libsemigroups::IsBipartition "IsBipartition", for example, will be present in
every version of `libsemigroups` starting with the number `3`, but will be
removed in the first version starting with `4`. The documentation should
indicate an alternative for the deprecated entity, in the example of
`IsBipartition<Thing>` this is `std::is_same_v<Thing, Bipartition>`.
