/* A hystrix like circuit breaker for reason/ocaml */

open Type;

let state = OPEN
let forced = FALSE
let windowDuration = 1000
let numBuckets = 10
let timeoutDuration = 3000
let errorThreshold = 50
let volumeThreshold = 5
let interval = 1
let bucketIndex = 0;
let timeout = None
let buckets = None

let bucketDuration = windowDuration / numBuckets

let extractBuckets = () =>{

switch(buckets){
               | None => []
               | Some(x) => x
               };
}

let isOpen = () => {

switch(state){
| OPEN => true
| HALF_OPEN => false
| CLOSED => false
}
}

let createBucket = () => {

{ failures: 0, successes: 0, timeouts: 0, shortCircuits: 0 }
}

let destroy = () => {

[%bs.raw {|  clearInterval(interval)  |}];
}

let lastBucket = () => {

let reversed = List.rev(extractBuckets());
List.hd(reversed)
}

let forceOpen = () => {

let forced = state;
let state = OPEN
}

let forceClose = () => {

let forced = state;
let state = CLOSED
}

let unForce = () => {

let state = forced;
let forced = FALSE
}

let executeCommand = (command) => {

    [%bs.raw {| setTimeout(increment('timeouts', timeout), timeoutDuration) |}];

    command();
}


let executeFallback = (fallback) => {

   fallback();

   let buckets = switch(buckets){
   | None => None
   | Some(b) => let reversed = List.rev (b);
                   let last = List.hd (reversed);
                   let middle = List.tl (reversed);
                   let modified = List.rev (middle);

                    last.shortCircuits + 1;

                    let buckets =  [last];
                    Some(buckets)
   }
}

let calculateMetrics = () => {

let totalCount = 0;
let errorCount = 0;
let errorPercentage = 0;
let first = 0;

let buckets = switch(buckets){
   | None => []
   | Some(b) => b
   }

let last = List.length (buckets);

for(x in first to last){

let bucket = List.nth(buckets, x);
let errors = bucket.failures + bucket.timeouts;

let errorCount = errorCount + errors;
let totalCount = errors + bucket.successes;
}

let metrics = {

 totalCount: totalCount,
 errorCount: errorCount,
 errorPercentage: errorPercentage
};

 metrics
}

let updateState = (buckets) => {

let metrics = calculateMetrics();

if(state == HALF_OPEN){

let last = lastBucket();

let state = switch(last.successes == 0 && metrics.errorCount > 0){
| true =>  OPEN
| false => CLOSED
};

}else{

let overErrorThreshold = metrics.errorPercentage > errorThreshold;
let overVolumeThreshold = metrics.totalCount > volumeThreshold;
let overThreshold = overVolumeThreshold && overErrorThreshold;

let state =
if(overThreshold == true){

 OPEN

}else{
state
};

};
state;
}


let increment = (prop) =>  {

let timeout = switch (timeout) {
         | None => false
         | Some(t) => let bucket = lastBucket();

                bucket[prop] + 1;

                let buckets =  [bucket];
                let forced = None;

                if(forced == None){

                 updateState(buckets);

                 };

                [%bs.raw {| clearTimeout(t) |}];

                 false
         };
         timeout
}

let tick = (bucketIndex) => {

let len =
    switch(buckets){
    | None => 0
    | Some(b) => List.length(b)
    };

let buckets_list =
    switch(buckets){
    | None => []
    | Some(b) => b
    };

let buckets =
 switch(len > numBuckets){
 | true => let bucketl = List.tl(buckets_list);
            Some(bucketl)
 | false => buckets
 };

 bucketIndex + 1;

let bucketIndex =
    switch (bucketIndex > numBuckets) {
         | true => 0
         | false => bucketIndex
         };

let newbucket = createBucket();

let buckets =
    switch(buckets){
    | None => []
    | Some(b) => [newbucket]
    };

let buckets = Some(buckets);
(buckets, bucketIndex)
}

let startTicker = () => {

[%bs.raw {| interval = setInterval(tick(bucketIndex), bucketDuration) |}];

}

let run = (command, fallback) => {

switch state{
| OPEN => executeCommand(command)
| CLOSED => executeFallback(fallback)
}
}

