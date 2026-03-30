def isscalar:
    . == null or . == true or . == false or
    type == "number" or type == "string";

def flatten_keys:
    def _flatten_iterable:
        . as $in |
        reduce (
            [paths(isscalar)] |
            map(
                to_entries |del(
                    . as $x |
                    [$x[] |select(.value|type != "string") |.key] |first as $begin |
                    $x[] |select($begin and .key >= $begin)
                ) |
                map(.value) |join(".")
            ) |
            unique |
            map(split("."))[]
        ) as $p (
            null;
            ($in |getpath($p)) as $v |
                (if ($p|length) == 0 then [] else [$p |join(".")] end) as $target_path |
                if ($v|type) == "array" then
                    setpath($target_path; $v |map(.|flatten_keys))
                else
                    setpath($target_path; $v)
                end
        );
    if isscalar then .
    else . |_flatten_iterable end;

def index_keys:
    if type == "array" then
        map(index_keys)
    elif type == "object" then
        with_entries(
            .key = (.key|gsub("[^[:alnum:]_]"; "_")) |
            .value = (.value |index_keys)
        )
    else
        .
    end;
