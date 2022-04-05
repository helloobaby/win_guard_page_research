## NtProtectVirtualMemory

    if (CapturedBase > MM_HIGHEST_USER_ADDRESS) {

        //
        // Invalid base address.
        //

        return STATUS_INVALID_PARAMETER_2;
    }


	 Status = ObReferenceObjectByHandle ( ProcessHandle,
                                         PROCESS_VM_OPERATION, //相应的权限
                                         PsProcessType,
                                         PreviousMode,
                                         (PVOID *)&Process,
                                         NULL );

	 Status = MiProtectVirtualMemory (Process,
                                     &CapturedBase,
                                     &CapturedRegionSize,
                                     NewProtect,
                                     &LastProtect);


	if (FoundVad->u.VadFlags.NoChange == 1) {

        //
        // An attempt is made at changing the protection
        // of a secured VAD, check to see if the address range
        // to change allows the change.
        //

        Status = MiCheckSecuredVad (FoundVad,
                                    CapturedBase,
                                    CapturedRegionSize,
                                    ProtectionMask);

        if (!NT_SUCCESS (Status)) {
            goto ErrorFound;
        }
    }

    NewProtect = MiMakeProtectForNativePage (StartingAddressFor4k, 
                                                 NewProtect, 
                                                 Process);

    ProtectionMask = MiMakeProtectionMask(NewProtect);

## MiProtectVirtualMemory
	CapturedBase = *BaseAddress;
    CapturedRegionSize = *RegionSize;
    OriginalProtect = NewProtect;

	EndingAddress = (PVOID)(((ULONG_PTR)CapturedBase +
                                CapturedRegionSize - 1L) | (PAGE_SIZE - 1L));
    StartingAddress = (PVOID)PAGE_ALIGN(CapturedBase); //以页为单位，即使参数不对齐，内部也会对齐

                //
                // The PTE is a private page which is valid, if the
                // specified protection is no-access or guard page
                // remove the PTE from the working set.
                //

                if ((NewProtect & PAGE_NOACCESS) || (NewProtect & PAGE_GUARD)) {

                    //
                    // Remove the page from the working set.
                    //

                    Locked = MiRemovePageFromWorkingSet (PointerPte,
                                                         Pfn1,
                                                         &Process->Vm);

                    continue;
                }	

	Pfn1->OriginalPte.u.Soft.Protection = ProtectionMask;


## MiCheckSecuredVad
	    if (ProtectionMask < MM_SECURE_DELETE_CHECK) {
        if ((Vad->u.VadFlags.NoChange == 1) &&
            (Vad->u2.VadFlags2.SecNoChange == 1) &&
            (Vad->u.VadFlags.Protection != ProtectionMask)) {

            //
            // An attempt is made at changing the protection
            // of a SEC_NO_CHANGE section - return an error.
            //

            Status = STATUS_INVALID_PAGE_PROTECTION;
            goto done;
        }
    }
	