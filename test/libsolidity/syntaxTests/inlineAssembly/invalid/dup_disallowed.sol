contract C {
    function f() pure public {
        assembly {
            dup0()
            dup1()
            dup2()
            dup3()
            dup4()
            dup5()
            dup6()
            dup7()
            dup8()
            dup9()
            dup10()
            dup11()
            dup12()
            dup13()
            dup14()
            dup15()
            dup16()
            dup32()
        }
    }
}
// ----
// DeclarationError 4619: (75-79): Function not found.
// TypeError 5017: (94-98): The identifier "dup1" can not be used.
// TypeError 5017: (113-117): The identifier "dup2" can not be used.
// TypeError 5017: (132-136): The identifier "dup3" can not be used.
// TypeError 5017: (151-155): The identifier "dup4" can not be used.
// TypeError 5017: (170-174): The identifier "dup5" can not be used.
// TypeError 5017: (189-193): The identifier "dup6" can not be used.
// TypeError 5017: (208-212): The identifier "dup7" can not be used.
// TypeError 5017: (227-231): The identifier "dup8" can not be used.
// TypeError 5017: (246-250): The identifier "dup9" can not be used.
// TypeError 5017: (265-270): The identifier "dup10" can not be used.
// TypeError 5017: (285-290): The identifier "dup11" can not be used.
// TypeError 5017: (305-310): The identifier "dup12" can not be used.
// TypeError 5017: (325-330): The identifier "dup13" can not be used.
// TypeError 5017: (345-350): The identifier "dup14" can not be used.
// TypeError 5017: (365-370): The identifier "dup15" can not be used.
// TypeError 5017: (385-390): The identifier "dup16" can not be used.
// DeclarationError 4619: (405-410): Function not found.
