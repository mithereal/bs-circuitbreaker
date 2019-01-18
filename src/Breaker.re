/* A hystrix like circuit breaker for reason/ocaml */

open Type;

let state = ref(OPEN)
let forced = ref(FALSE)
let timeout = ref(None)
let buckets = ref(None)

let windowDuration = ref(1000)
let numBuckets = ref(10)
let timeoutDuration = ref(3000)
let errorThreshold = ref(50)
let volumeThreshold = ref(5)
let interval = ref(1)
let bucketIndex = ref(0)

let bucketDuration = windowDuration^ / numBuckets^

let extractBuckets = () =>{

switch(buckets^){
               | None => []
               | Some(x) => x
               };
}

let isOpen = () => {

switch(state^){
| OPEN => true
| HALF_OPEN => false
| CLOSED => false
| FALSE => false
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

forced := state^;
state := OPEN
}

let forceClose = () => {

forced := state^;
state := CLOSED
}

let unForce = () => {

state := state^;
forced := FALSE
}


let executeCommand = (command) => {

    [%bs.raw {| setTimeout(increment('timeouts', timeout), timeoutDuration) |}];

    command();
}


let executeFallback = (fallback) => {

   fallback();

   let buckets = switch(buckets^){
   | None => None
   | Some(b) => let reversed = List.rev (b);
                   let last = List.hd (reversed);
                   let middle = List.tl (reversed);
                   let modified = List.rev (middle);

                    last.shortCircuits + 1;

                    let new_buckets =  List.append(modified, [last]);
                    Some(new_buckets)
   };

   buckets := buckets
}

let calculateMetrics = () => {

let totalCount = 0;
let errorCount = 0;
let errorPercentage = 0;
let first = 0;

let buckets = extractBuckets();

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

state := switch(last.successes == 0 && metrics.errorCount > 0){
| true =>  OPEN
| false => CLOSED
};

}else{

let overErrorThreshold = metrics.errorPercentage > errorThreshold^;
let overVolumeThreshold = metrics.totalCount > volumeThreshold^;
let overThreshold = overVolumeThreshold && overErrorThreshold;

state :=
if(overThreshold == true){

OPEN
}else{

state
};

};

}


let increment = (prop) =>  {

let timeout = switch (timeout) {
         | None => false
         | Some(t) => let bucket = lastBucket();

                bucket[prop] + 1;

                let buckets =  List.append(modified, [bucket]);
                let forced = None;

                if(forced == None){

                 updateState(buckets);

                 };

                [%bs.raw {| clearTimeout(t) |}];

                buckets := buckets;
                forced := forced;

                 false
         };

timeout := timeout
}

let tick = (bucketIndex) => {

let len =
    switch(buckets){
    | None => 0
    | Some(b) => List.length(b)
    };


let buckets =
 switch(len > numBuckets){
 | true => let buckets = List.tl(extractBuckets());
            Some(buckets)
 | false => buckets
 };

 bucketIndex + 1;

let bucketIndex =
    switch (bucketIndex > numBuckets) {
         | true => 0
         | false => bucketIndex
         };

bucketIndex := bucketIndex

let newbucket = createBucket();

    switch(buckets){
    | None => None
    | Some(b) => let buckets = List.append(buckets, [newbucket]);
                     buckets := Some(buckets)
    };
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

