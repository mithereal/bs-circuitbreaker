
type state =
| FALSE
| CLOSED
| OPEN
| HALF_OPEN
| None

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

type forced =
| FALSE
| CLOSED
| OPEN
| HALF_OPEN
| None

type timeout = option(bool)

type buckets = option(list(bucket))
