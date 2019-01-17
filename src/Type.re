
type state =
| FALSE
| CLOSED
| OPEN
| HALF_OPEN

type bucket = {
failures: int,
successes: int,
timeouts: int,
shortCircuits: int
}

type metrics = {
totalCount: int,
errorCount: int,
errorPercentage: int
}

type timeout = option(bool)

type buckets = option(list(bucket))
