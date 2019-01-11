type state
| CLOSED
| OPEN
| HALF_OPEN

type bucket = {
failures: int,
successes: int,
timeouts: int,
shortCircuits: int
}

type forced
| FALSE
| CLOSED
| OPEN
| HALF_OPEN

let state = OPEN
let forced = FALSE

type buckets = option(list(bucket))

let windowDuration = 1000
let numBuckets = 10
let timeoutDuration = 3000
let errorThreshold = 50
let volumeThreshold = 5
let buckets = None

let isOpen = () => {

switch state
| OPEN => true
| HALF_OPEN => false
| CLOSED => false

}


let run = (command, fallback) => {

switch state
| OPEN => executeCommand(command)
| CLOSED => executeFallback(fallback)

}

let forceClose = () => {
forced = state
state = CLOSED
}

let forceOpen = () => {
forced = state
state = OPEN
}

let unforce = () => {
state = forced
forced = FALSE
}


let tick = (bucketIndex) => {
let len = List.length buckets

 buckets =
 switch (len > numBuckets)
 | true => buckets.shift()
 | _ => buckets

 bucketIndex + 1

switch (bucketIndex > numBuckets) {
         | true => 0
         | false => bucketIndex
         }

         buckets.push(createBucket())

}

let startTicker = () => {

let bucketIndex = 0;
let bucketDuration = windowDuration / numBuckets

[%bs.raw {| setInterval(tick(bucketIndex), bucketDuration) |}];

}

let createBucket = () => {
{ failures: 0, successes: 0, timeouts: 0, shortCircuits: 0 }
}

let lastBucket = () => {
let reversed = List.rev buckets
List.hd reversed
}

let increment = (prop, timeout) =>  {
prop

}

let executeCommand = (command) => {
    let timeout = false

    timeout = [%bs.raw {| setTimeout(increment('timeouts', timeout), timeoutDuration) |}];

    command(increment('successes', timeout), increment('failures', timeout));
}


let executeFallback = (fallback) => {
   fallback()

   let reversed = List.rev buckets
   let hd = List.hd reversed
   let tl = List.tl reversed
   let modified = List.rev tl
   let bucket = hd

    bucket.shortCircuits + 1

    buckets = modified ++ bucket
}

let calculateMetrics = () => {

}

let updateState = () => {

}