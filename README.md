# bs-circuitbreaker

A circuit breaker for dealing with reason/bs requests.

sometimes api requests timeout or have other errors, this library is desigened to deal with that in a circiutbreaker fashion.

we pass the fun we want to run and a failure fun, when the threshold of timeouts or types of failures is exceeded the failure fun is run.

timeouts and other settings can be manupuilated via instantiation then using CircuitBreaker.propname to set the property where propname is from the following list [windowDuration:integer,numBuckets:integer,timeoutDuration:integer,errorThreshold:integer, volumeThreshold:integer]
ex. CircuitBreaker.numBuckets = 50

###### Example
let try = () => {

 [%bs.raw {| setTimeout(fn(){ console.log("try is executing for 10000 whereas the timeout is set at 5000")}, 10000) |}];
 
}

let catch = () => {
Js.log("an error has occurred")
}

CircuitBreaker.run(try, catch)
