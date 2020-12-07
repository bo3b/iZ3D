' DHTML Editing Component Constants for VBScript
' Copyright 1998-1999 Microsoft Corporation.  All rights reserved.
'

'
' Command IDs
'
DECMD_BOLD =                      5000
DECMD_COPY =                      5002
DECMD_CUT =                       5003
DECMD_DELETE =                    5004
DECMD_DELETECELLS =               5005
DECMD_DELETECOLS =                5006
DECMD_DELETEROWS =                5007
DECMD_FINDTEXT =                  5008
DECMD_FONT =                      5009
DECMD_GETBACKCOLOR =              5010
DECMD_GETBLOCKFMT =               5011
DECMD_GETBLOCKFMTNAMES =          5012
DECMD_GETFONTNAME =               5013
DECMD_GETFONTSIZE =               5014
DECMD_GETFORECOLOR =              5015
DECMD_HYPERLINK =                 5016
DECMD_IMAGE =                     5017
DECMD_INDENT =                    5018
DECMD_INSERTCELL =                5019
DECMD_INSERTCOL =                 5020
DECMD_INSERTROW =                 5021
DECMD_INSERTTABLE =               5022
DECMD_ITALIC =                    5023
DECMD_JUSTIFYCENTER =             5024
DECMD_JUSTIFYLEFT =               5025
DECMD_JUSTIFYRIGHT =              5026
DECMD_LOCK_ELEMENT =              5027
DECMD_MAKE_ABSOLUTE =             5028
DECMD_MERGECELLS =                5029
DECMD_ORDERLIST =                 5030
DECMD_OUTDENT =                   5031
DECMD_PASTE =                     5032
DECMD_REDO =                      5033
DECMD_REMOVEFORMAT =              5034
DECMD_SELECTALL =                 5035
DECMD_SEND_BACKWARD =             5036
DECMD_BRING_FORWARD =             5037
DECMD_SEND_BELOW_TEXT =           5038
DECMD_BRING_ABOVE_TEXT =          5039
DECMD_SEND_TO_BACK =              5040
DECMD_BRING_TO_FRONT =            5041
DECMD_SETBACKCOLOR =              5042
DECMD_SETBLOCKFMT =               5043
DECMD_SETFONTNAME =               5044
DECMD_SETFONTSIZE =               5045
DECMD_SETFORECOLOR =              5046
DECMD_SPLITCELL =                 5047
DECMD_UNDERLINE =                 5048
DECMD_UNDO =                      5049
DECMD_UNLINK =                    5050
DECMD_UNORDERLIST =               5051
DECMD_PROPERTIES =                5052

'
' Enums
'

' OLECMDEXECOPT  
OLECMDEXECOPT_DODEFAULT =         0 
OLECMDEXECOPT_PROMPTUSER =        1
OLECMDEXECOPT_DONTPROMPTUSER =    2

' DHTMLEDITCMDF
DECMDF_NOTSUPPORTED =             0 
DECMDF_DISABLED =                 1 
DECMDF_ENABLED =                  3
DECMDF_LATCHED =                  7
DECMDF_NINCHED =                  11

' DHTMLEDITAPPEARANCE
DEAPPEARANCE_FLAT =               0
DEAPPEARANCE_3D =                 1 

' OLE_TRISTATE
OLE_TRISTATE_UNCHECKED =          0
OLE_TRISTATE_CHECKED =            1
OLE_TRISTATE_GRAY =               2

'
' Error Return Values
'
DE_E_INVALIDARG =                 &h5
DE_E_ACCESS_DENIED =              &h46
DE_E_PATH_NOT_FOUND =             &h80070003
DE_E_FILE_NOT_FOUND =             &h80070002
DE_E_UNEXPECTED =                 &h8000ffff
DE_E_DISK_FULL =                  &h80070027
DE_E_NOTSUPPORTED =               &h80040100
DE_E_FILTER_FRAMESET =            &h80100001
DE_E_FILTER_SERVERSCRIPT =        &h80100002
DE_E_FILTER_MULTIPLETAGS =        &h80100004
DE_E_FILTER_SCRIPTLISTING =       &h80100008
DE_E_FILTER_SCRIPTLABEL =         &h80100010
DE_E_FILTER_SCRIPTTEXTAREA =      &h80100020
DE_E_FILTER_SCRIPTSELECT =        &h80100040
DE_E_URL_SYNTAX =                 &h800401E4
DE_E_INVALID_URL =                &h800C0002
DE_E_NO_SESSION =                 &h800C0003
DE_E_CANNOT_CONNECT =             &h800C0004
DE_E_RESOURCE_NOT_FOUND =         &h800C0005
DE_E_OBJECT_NOT_FOUND =           &h800C0006
DE_E_DATA_NOT_AVAILABLE =         &h800C0007
DE_E_DOWNLOAD_FAILURE =           &h800C0008
DE_E_AUTHENTICATION_REQUIRED =    &h800C0009
DE_E_NO_VALID_MEDIA =             &h800C000A
DE_E_CONNECTION_TIMEOUT =         &h800C000B
DE_E_INVALID_REQUEST =            &h800C000C
DE_E_UNKNOWN_PROTOCOL =           &h800C000D
DE_E_SECURITY_PROBLEM =           &h800C000E
DE_E_CANNOT_LOAD_DATA =           &h800C000F
DE_E_CANNOT_INSTANTIATE_OBJECT =  &h800C0010
DE_E_REDIRECT_FAILED =            &h800C0014
DE_E_REDIRECT_TO_DIR =            &h800C0015
DE_E_CANNOT_LOCK_REQUEST =        &h800C0016


'' SIG '' Begin signature block
'' SIG '' MIIXLwYJKoZIhvcNAQcCoIIXIDCCFxwCAQExCzAJBgUr
'' SIG '' DgMCGgUAMGcGCisGAQQBgjcCAQSgWTBXMDIGCisGAQQB
'' SIG '' gjcCAR4wJAIBAQQQTvApFpkntU2P5azhDxfrqwIBAAIB
'' SIG '' AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFEFpEqE5AaWv
'' SIG '' 1ARU9u8oguYWNDTKoIISJDCCBGAwggNMoAMCAQICCi6r
'' SIG '' EdxQ/1ydy8AwCQYFKw4DAh0FADBwMSswKQYDVQQLEyJD
'' SIG '' b3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAu
'' SIG '' MR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
'' SIG '' ITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0
'' SIG '' eTAeFw0wNzA4MjIyMjMxMDJaFw0xMjA4MjUwNzAwMDBa
'' SIG '' MHkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
'' SIG '' dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
'' SIG '' aWNyb3NvZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1p
'' SIG '' Y3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBMIIBIjANBgkq
'' SIG '' hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt3l91l2zRTmo
'' SIG '' NKwx2vklNUl3wPsfnsdFce/RRujUjMNrTFJi9JkCw03Y
'' SIG '' SWwvJD5lv84jtwtIt3913UW9qo8OUMUlK/Kg5w0jH9FB
'' SIG '' JPpimc8ZRaWTSh+ZzbMvIsNKLXxv2RUeO4w5EDndvSn0
'' SIG '' ZjstATL//idIprVsAYec+7qyY3+C+VyggYSFjrDyuJSj
'' SIG '' zzimUIUXJ4dO3TD2AD30xvk9gb6G7Ww5py409rQurwp9
'' SIG '' YpF4ZpyYcw2Gr/LE8yC5TxKNY8ss2TJFGe67SpY7UFMY
'' SIG '' zmZReaqth8hWPp+CUIhuBbE1wXskvVJmPZlOzCt+M26E
'' SIG '' RwbRntBKhgJuhgCkwIffUwIDAQABo4H6MIH3MBMGA1Ud
'' SIG '' JQQMMAoGCCsGAQUFBwMDMIGiBgNVHQEEgZowgZeAEFvQ
'' SIG '' cO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQLEyJDb3B5
'' SIG '' cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAuMR4w
'' SIG '' HAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xITAf
'' SIG '' BgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0eYIP
'' SIG '' AMEAizw8iBHRPvZj7N9AMA8GA1UdEwEB/wQFMAMBAf8w
'' SIG '' HQYDVR0OBBYEFMwdznYAcFuv8drETppRRC6jRGPwMAsG
'' SIG '' A1UdDwQEAwIBhjAJBgUrDgMCHQUAA4IBAQB7q65+Siby
'' SIG '' zrxOdKJYJ3QqdbOG/atMlHgATenK6xjcacUOonzzAkPG
'' SIG '' yofM+FPMwp+9Vm/wY0SpRADulsia1Ry4C58ZDZTX2h6t
'' SIG '' KX3v7aZzrI/eOY49mGq8OG3SiK8j/d/p1mkJkYi9/uEA
'' SIG '' uzTz93z5EBIuBesplpNCayhxtziP4AcNyV1ozb2AQWtm
'' SIG '' qLu3u440yvIDEHx69dLgQt97/uHhrP7239UNs3DWkuNP
'' SIG '' tjiifC3UPds0C2I3Ap+BaiOJ9lxjj7BauznXYIxVhBoz
'' SIG '' 9TuYoIIMol+Lsyy3oaXLq9ogtr8wGYUgFA0qvFL0QeBe
'' SIG '' MOOSKGmHwXDi86erzoBCcnYOMIIEejCCA2KgAwIBAgIK
'' SIG '' YQYngQAAAAAACDANBgkqhkiG9w0BAQUFADB5MQswCQYD
'' SIG '' VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
'' SIG '' A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0
'' SIG '' IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3NvZnQg
'' SIG '' Q29kZSBTaWduaW5nIFBDQTAeFw0wODEwMjIyMTI0NTVa
'' SIG '' Fw0xMDAxMjIyMTM0NTVaMIGDMQswCQYDVQQGEwJVUzET
'' SIG '' MBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
'' SIG '' bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0
'' SIG '' aW9uMQ0wCwYDVQQLEwRNT1BSMR4wHAYDVQQDExVNaWNy
'' SIG '' b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEB
'' SIG '' AQUAA4IBDwAwggEKAoIBAQC9crSJ5xyfhcd0uGBcAzY9
'' SIG '' nP2ZepopRiKwp4dT7e5GOsdbBQtXqLfKBczTTHdHcIWz
'' SIG '' 5cvfZ+ej/XQnk2ef14oDRDDG98m6yTodCFZETxcIDfm0
'' SIG '' GWiqJBz7BVeF6cVOByE3p+vOLC+2Qs0hBafW5tMoV8cb
'' SIG '' es4pNgfNnlXMu/Ei66gjpA0pwvvQw1o+Yz3HLEkLe3mF
'' SIG '' 8Ijvcb1DWuOjsw3zVfsl4OIg0+eaXpSlMy0of1cbVWoM
'' SIG '' MkTvZmxv8Dic7wKtmqHdmAcQDjwYaeJ5TkYU4LmM0HVt
'' SIG '' nKwAnC1C9VG4WvR4RYPpLnwru13NGWEorZRDCsVqQv+1
'' SIG '' Mq6kKSLeFujTAgMBAAGjgfgwgfUwEwYDVR0lBAwwCgYI
'' SIG '' KwYBBQUHAwMwHQYDVR0OBBYEFCPRcypMvfvlIfpxHpkV
'' SIG '' 0Rf5xKaKMA4GA1UdDwEB/wQEAwIHgDAfBgNVHSMEGDAW
'' SIG '' gBTMHc52AHBbr/HaxE6aUUQuo0Rj8DBEBgNVHR8EPTA7
'' SIG '' MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
'' SIG '' cGtpL2NybC9wcm9kdWN0cy9DU1BDQS5jcmwwSAYIKwYB
'' SIG '' BQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8vd3d3
'' SIG '' Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL0NTUENBLmNy
'' SIG '' dDANBgkqhkiG9w0BAQUFAAOCAQEAQynPY71s43Ntw5nX
'' SIG '' bQyIO8ZIc3olziziN3udNJ+9I86+39hceRFrE1EgAWO5
'' SIG '' cvcI48Z9USoWKNTR55sqzxgN0hNxkSnsVr351sUNL69l
'' SIG '' LW1NRSlWcoRPP9JqHUFiqXlcjvDHd4rLAiguncecK+W5
'' SIG '' Kgnd7Jfi5XqNXhCIU6HdYE93mHFgqFs5kdOrEh8F6cNF
'' SIG '' qdPCUbmvuNz8BoQA9HSj2//MHaAjBQfkJzXCl5AZqoJg
'' SIG '' J+j7hCse0QTLjs+CDdeoTUNAddLe3XfvilxrD4dkj7S6
'' SIG '' t7qrZ1QhRapKaOdUXosUXGd47JBcAxCRCJ0kIJfo3wAR
'' SIG '' cKn5snJwt67iwp8WAjCCBJ0wggOFoAMCAQICCmFHUroA
'' SIG '' AAAAAAQwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UEBhMC
'' SIG '' VVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcT
'' SIG '' B1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
'' SIG '' b3JhdGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IFRpbWVz
'' SIG '' dGFtcGluZyBQQ0EwHhcNMDYwOTE2MDE1MzAwWhcNMTEw
'' SIG '' OTE2MDIwMzAwWjCBpjELMAkGA1UEBhMCVVMxEzARBgNV
'' SIG '' BAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQx
'' SIG '' HjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEn
'' SIG '' MCUGA1UECxMebkNpcGhlciBEU0UgRVNOOkQ4QTktQ0ZD
'' SIG '' Qy01NzlDMScwJQYDVQQDEx5NaWNyb3NvZnQgVGltZXN0
'' SIG '' YW1waW5nIFNlcnZpY2UwggEiMA0GCSqGSIb3DQEBAQUA
'' SIG '' A4IBDwAwggEKAoIBAQCbbdyGUegyOzc6liWyz2/uYbVB
'' SIG '' 0hg7Wp14Z7r4H9kIVZKIfuNBU/rsKFT+tdr+cDuVJ0h+
'' SIG '' Q6AyLyaBSvICdnfIyan4oiFYfg29Adokxv5EEQU1OgGo
'' SIG '' 6lQKMyyH0n5Bs+gJ2bC+45klprwl7dfTjtv0t20bSQvm
'' SIG '' 08OHbu5GyX/zbevngx6oU0Y/yiR+5nzJLPt5FChFwE82
'' SIG '' a1Map4az5/zhwZ9RCdu8pbv+yocJ9rcyGb7hSlG8vHys
'' SIG '' LJVql3PqclehnIuG2Ju9S/wnM8FtMqzgaBjYbjouIkPR
'' SIG '' +Y/t8QABDWTAyaPdD/HI6VTKEf/ceCk+HaxYwNvfqtyu
'' SIG '' ZRvTnbxnAgMBAAGjgfgwgfUwHQYDVR0OBBYEFE8YiYrS
'' SIG '' ygB4xuxZDQ/9fMTBIoDeMB8GA1UdIwQYMBaAFG/oTj+X
'' SIG '' uTSrS4aPvJzqrDtBQ8bQMEQGA1UdHwQ9MDswOaA3oDWG
'' SIG '' M2h0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3Js
'' SIG '' L3Byb2R1Y3RzL3RzcGNhLmNybDBIBggrBgEFBQcBAQQ8
'' SIG '' MDowOAYIKwYBBQUHMAKGLGh0dHA6Ly93d3cubWljcm9z
'' SIG '' b2Z0LmNvbS9wa2kvY2VydHMvdHNwY2EuY3J0MBMGA1Ud
'' SIG '' JQQMMAoGCCsGAQUFBwMIMA4GA1UdDwEB/wQEAwIGwDAN
'' SIG '' BgkqhkiG9w0BAQUFAAOCAQEANyce9YxA4PZlJj5kxJC8
'' SIG '' PuNXhd1DDUCEZ76HqCra3LQ2IJiOM3wuX+BQe2Ex8xoT
'' SIG '' 3oS96mkcWHyzG5PhCCeBRbbUcMoUt1+6V+nUXtA7Q6q3
'' SIG '' P7baYYtxz9R91Xtuv7TKWjCR39oKDqM1nyVhTsAydCt6
'' SIG '' BpRyAKwYnUvlnivFOlSspGDYp/ebf9mpbe1Ea7rc4BL6
'' SIG '' 8K2HDJVjCjIeiU7MzH6nN6X+X9hn+kZL0W0dp33SvgL/
'' SIG '' 826C84d0xGnluXDMS2WjBzWpRJ6EfTlu/hQFvRpQIbU+
'' SIG '' n/N3HI/Cmp1X4Wl9aeiDzwJvKiK7NzM6cvrWMB2RrfZQ
'' SIG '' GusT3jrFt1zNszCCBJ0wggOFoAMCAQICEGoLmU/AACWr
'' SIG '' EdtFH1h6Z6IwDQYJKoZIhvcNAQEFBQAwcDErMCkGA1UE
'' SIG '' CxMiQ29weXJpZ2h0IChjKSAxOTk3IE1pY3Jvc29mdCBD
'' SIG '' b3JwLjEeMBwGA1UECxMVTWljcm9zb2Z0IENvcnBvcmF0
'' SIG '' aW9uMSEwHwYDVQQDExhNaWNyb3NvZnQgUm9vdCBBdXRo
'' SIG '' b3JpdHkwHhcNMDYwOTE2MDEwNDQ3WhcNMTkwOTE1MDcw
'' SIG '' MDAwWjB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
'' SIG '' aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UE
'' SIG '' ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQD
'' SIG '' ExpNaWNyb3NvZnQgVGltZXN0YW1waW5nIFBDQTCCASIw
'' SIG '' DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANw3bvuv
'' SIG '' yEJKcRjIzkg+U8D6qxS6LDK7Ek9SyIPtPjPZSTGSKLaR
'' SIG '' ZOAfUIS6wkvRfwX473W+i8eo1a5pcGZ4J2botrfvhbnN
'' SIG '' 7qr9EqQLWSIpL89A2VYEG3a1bWRtSlTb3fHev5+Dx4Df
'' SIG '' f0wCN5T1wJ4IVh5oR83ZwHZcL322JQS0VltqHGP/gHw8
'' SIG '' 7tUEJU05d3QHXcJc2IY3LHXJDuoeOQl8dv6dbG564Ow+
'' SIG '' j5eecQ5fKk8YYmAyntKDTisiXGhFi94vhBBQsvm1Go1s
'' SIG '' 7iWbE/jLENeFDvSCdnM2xpV6osxgBuwFsIYzt/iUW4RB
'' SIG '' hFiFlG6wHyxIzG+cQ+Bq6H8mjmsCAwEAAaOCASgwggEk
'' SIG '' MBMGA1UdJQQMMAoGCCsGAQUFBwMIMIGiBgNVHQEEgZow
'' SIG '' gZeAEFvQcO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQL
'' SIG '' EyJDb3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENv
'' SIG '' cnAuMR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRp
'' SIG '' b24xITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhv
'' SIG '' cml0eYIPAMEAizw8iBHRPvZj7N9AMBAGCSsGAQQBgjcV
'' SIG '' AQQDAgEAMB0GA1UdDgQWBBRv6E4/l7k0q0uGj7yc6qw7
'' SIG '' QUPG0DAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMAQTAL
'' SIG '' BgNVHQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zANBgkq
'' SIG '' hkiG9w0BAQUFAAOCAQEAlE0RMcJ8ULsRjqFhBwEOjHBF
'' SIG '' je9zVL0/CQUt/7hRU4Uc7TmRt6NWC96Mtjsb0fusp8m3
'' SIG '' sVEhG28IaX5rA6IiRu1stG18IrhG04TzjQ++B4o2wet+
'' SIG '' 6XBdRZ+S0szO3Y7A4b8qzXzsya4y1Ye5y2PENtEYIb92
'' SIG '' 3juasxtzniGI2LS0ElSM9JzCZUqaKCacYIoPO8cTZXhI
'' SIG '' u8+tgzpPsGJY3jDp6Tkd44ny2jmB+RMhjGSAYwYElvKa
'' SIG '' AkMve0aIuv8C2WX5St7aA3STswVuDMyd3ChhfEjxF5wR
'' SIG '' ITgCHIesBsWWMrjlQMZTPb2pid7oZjeN9CKWnMywd1RR
'' SIG '' OtZyRLIj9jGCBHcwggRzAgEBMIGHMHkxCzAJBgNVBAYT
'' SIG '' AlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQH
'' SIG '' EwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
'' SIG '' cG9yYXRpb24xIzAhBgNVBAMTGk1pY3Jvc29mdCBDb2Rl
'' SIG '' IFNpZ25pbmcgUENBAgphBieBAAAAAAAIMAkGBSsOAwIa
'' SIG '' BQCggaIwGQYJKoZIhvcNAQkDMQwGCisGAQQBgjcCAQQw
'' SIG '' HAYKKwYBBAGCNwIBCzEOMAwGCisGAQQBgjcCARUwIwYJ
'' SIG '' KoZIhvcNAQkEMRYEFBgGNI3a6+0sYv8d1xJmebup3/sV
'' SIG '' MEIGCisGAQQBgjcCAQwxNDAyoBiAFgBEAEgAdABtAGwA
'' SIG '' ZQBkAC4AdgBiAHOhFoAUaHR0cDovL21pY3Jvc29mdC5j
'' SIG '' b20wDQYJKoZIhvcNAQEBBQAEggEAqGGTtawKp0J8WdGN
'' SIG '' 4bwZ9wNod5jSJIaNc3Suxo3HiKHHZg/HXwdr8C8JKZac
'' SIG '' f/ZxE+Am+8jD77/8xMN9YRrIa8W2bs+gCKNxseEEXpUX
'' SIG '' tlKjZkRJMCQ83QR3ytJNpgiHFTiaLBTmFxsFHM98NG5x
'' SIG '' d3YdOntmsGzMBi9RPA+wAZbqsBDHjkJSHj16276gm3qh
'' SIG '' S+YwGS+ZYBxzKnoGj+F8wfJ/soSuBowI3aBYOrb+V5Jd
'' SIG '' FH1ljPXaIj/iu0usPaGvlyMZs9AsL9dD2SShvMtlIFXf
'' SIG '' +IRd/J+ebNiY70wvYXZmJ+dvjnLucLbAxUsEzURDpNhM
'' SIG '' G+y4tqgaM7rVv8frGKGCAh8wggIbBgkqhkiG9w0BCQYx
'' SIG '' ggIMMIICCAIBATCBhzB5MQswCQYDVQQGEwJVUzETMBEG
'' SIG '' A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
'' SIG '' ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
'' SIG '' MSMwIQYDVQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5n
'' SIG '' IFBDQQIKYUdSugAAAAAABDAHBgUrDgMCGqBdMBgGCSqG
'' SIG '' SIb3DQEJAzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkF
'' SIG '' MQ8XDTA5MTAwNzE5NTQ0MFowIwYJKoZIhvcNAQkEMRYE
'' SIG '' FKr5UYtsmoJiEcuUzba3CpPHd9I2MA0GCSqGSIb3DQEB
'' SIG '' BQUABIIBAH2bP9FLkdWjV5zIR7hC9aSNeDeZu4EiL2Fa
'' SIG '' hlILwnG8Vr5D5fyuTEyZ6XwncaKrEwyKCoTtOqrupdyz
'' SIG '' 0CcxI8loWWzNJlRiukhz1nhWylxRUOw4D9mf8ElY9vpy
'' SIG '' qB83N5bSMitr4GwtN/mkJQ+fng+PK9oLagww0R7MkUz8
'' SIG '' q0FRTEFZjxBfLjxia7PMdB9NpAGJAvWUkol2mU653TN5
'' SIG '' FyT5OunozKHjH+9srjVOwewSZD9at25AYOS38Tel+qh6
'' SIG '' 0tJpnEfOPvW67xPDfmlQ6oto2+kqZIr8By5fPkuGiADz
'' SIG '' dsQBMY9Afb9mnAlo2FO+woQRji7wWfg+54VSpEkqFbg=
'' SIG '' End signature block