# bs-circuitbreaker

A circuit breaker for dealing with Reason/BuckleScript requests.

Sometimes API requests time out or have other errors. This library is desigened to deal with that in a circiutbreaker fashion.

We pass the function we want to run and a failure function. When the threshold of timeouts or types of failures is exceeded the failure fun is run.

Timeouts and other settings can be manupuilated via instantiation, then using `CircuitBreaker.propname` to set the property where propname is one of the following:

* windowDuration: int
* numBuckets: int
* timeoutDuration: int
* errorThreshold: int
* volumeThreshold: int

ex. `CircuitBreaker.numBuckets = 50`

### Example

```reason
let try = () => {
 [%bs.raw {| setTimeout(fn(){ console.log("try is executing for 10000 whereas the timeout is set at 5000")}, 10000) |}];
}

let catch = () => {
  Js.log("an error has occurred")
}

CircuitBreaker.run(try, catch)
```
