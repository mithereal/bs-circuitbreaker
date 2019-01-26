# bs-circuitbreaker

A circuit breaker for dealing with reason/bs requests.

sometimes api requests timeout or have other errors, this library is desigened to deal with that in a circiutbreaker fashion.

we pass the fun we want to run and a failure fun, when the threshold of timeouts or types of failures is exceeded the failure fun is run.

###### Example
let try = () => {

 [%bs.raw {| setTimeout(fn(){ console.log("try is executing for 10000")}, 10000) |}];
 
}

let catch = () => {
Js.log("an error has occurred")
}

CircuitBreaker.run(try, catch)
