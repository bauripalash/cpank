# Pankti Internal Memo #2 

```
Title: Pankti Import and Module System
Author: Palash Bauri <me [at] palashbauri [dot] in>
		<palashbauri1 [at] gmail [dot] com>
Status: Draft
Created: April 23, 2023 16:25:04 IST
Last Modified: April 10, 2023 10:35:30 IST
```

## Abstract
This memo describes and mentions all the standard library and builtin functions
which must be present in all Implementation of Pankti Programming Language

## Standard Library
### 1. OS 
#### Description:
Utility functions for fetching information related to the current operating 
system.

#### Module Name: os / ওএস

#### Functions
os.**name()**

Returns the name of operating system presently being used.
	On Windows returns 'windows' 
	On Linux based distribution and on other Unix-like OS, returns 'unix'
	On MacOS returns 'macos'
	On Android returns 'android'
	On Others or if failed to detect, returns 'unknown'

* [Count of Arguments: 0] 
* [Arguments: None] 
* [Return: String]



2. arch 

* Count of Arguments: 0
* Arguments: None
* Return: String

On x86_64 / amd64 / 64-bit  systems, it returns '64'. 
On x86 / 32-bit systems, it returns '32'. 
On arm systems, it returns 'arm'. 
On other systems or if failed to detect, it returns 'unknown'. 

3. user 

* [Count of Arguments: 0] 
* [Arguments: None] 
* [Return: String]

