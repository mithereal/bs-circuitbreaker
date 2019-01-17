
type state =
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

type forced =
| FALSE
| CLOSED
| OPEN
| HALF_OPEN

type timeout = option(boolean)

type buckets = option(list(bucket))
