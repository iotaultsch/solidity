contract C {
    function f() pure public {
        assembly {
            jumpi(2, 1)
        }
    }
}
// ----
// TypeError 5017: (75-80): The identifier "jumpi" can not be used.
