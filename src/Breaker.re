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

[%bs.raw {|  clearInterval(interval^)  |}];
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

   /* [%bs.raw {| setTimeout(increment('timeouts'), timeoutDuration^) |}]; */

    command();
}


let executeFallback = (fallback) => {

   fallback();

   let bs = switch(buckets^){
   | None => None
   | Some(b) => let reversed = List.rev (b);
                   let last = List.hd (reversed);
                   let middle = List.tl (reversed);
                   let modified = List.rev (middle);

                    let newbucket:bucket = {
                    failures: last.failures,
                    successes: last.successes,
                    timeouts: last.timeouts,
                    shortCircuits: last.shortCircuits + 1
                    }

                    let new_buckets =  List.append(modified, [newbucket]);
                    Some(new_buckets)
   };

   buckets := bs
}

let calculateMetrics = () => {

let totalCount = ref(0);
let errorCount = ref(0);
let errorPercentage = ref(0);

let buckets = extractBuckets();

let last = List.length (buckets);

for(x in 0 to last){

let bucket = List.nth(buckets, x);
let errors = bucket.failures + bucket.timeouts;

errorCount := errorCount^ + errors;
totalCount := errors + bucket.successes;
}

let metrics = {

 totalCount: totalCount^,
 errorCount: errorCount^,
 errorPercentage: errorPercentage^
};

 metrics
}

let updateState = () => {

let metrics = calculateMetrics();

if(state^ == HALF_OPEN){

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

state^
};

};

}

let increment = (prop) =>  {

let tO = switch (timeout^) {
         | None => None
         | Some(t) => let bucket = lastBucket();

           let bs = switch(buckets^){
            | None => None
            | Some(b) => let reversed = List.rev (b);
                            let last = List.hd (reversed);
                            let middle = List.tl (reversed);
                            let modified = List.rev (middle);


           let b = switch(t){
            | "failures" => bucket.failures + 1;
            | "successes" => bucket.successes + 1;
            | "timeouts" => bucket.timeouts + 1;
            | "shortCircuits" => bucket.shortCircuits + 1;
            | _ => 0
            }

                let bucket_list =  List.append(modified, [bucket]);
                let forced = None;

                if(forced == None){

                 updateState();

                 };

                [%bs.raw {| clearTimeout(t) |}];

               /* buckets := None; */
               /* forced := None; */

         };
         None
         };

 timeout := tO
}

let tick = (bucketIndex) => {

let len =
    switch(buckets^){
    | None => 0
    | Some(b) => List.length(b)
    };


let buckets =
 switch(len > numBuckets^){
 | true => let buckets = List.tl(extractBuckets());
            Some(buckets)
 | false => buckets^
 };

 bucketIndex := bucketIndex^ + 1;

let bI =
    switch (bucketIndex^ > numBuckets^) {
         | true => 0
         | false => bucketIndex^
         };

 bucketIndex := bI

let newbucket = createBucket();

    switch(buckets){
    | None => None
    | Some(b) => let bs = List.append(b, [newbucket]);
                    /* buckets := Some(bs) */
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

