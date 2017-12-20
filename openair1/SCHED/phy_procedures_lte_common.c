/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file phy_procedures_lte_eNB.c
* \brief Implementation of common utilities for eNB/UE procedures from 36.213 LTE specifications
* \author R. Knopp, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr
* \note
* \warning
*/
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"

#ifdef LOCALIZATION
#include <sys/time.h>
#endif

void get_Msg3_alloc(LTE_DL_FRAME_PARMS *frame_parms,
                    unsigned char current_subframe,
                    unsigned int current_frame,
                    unsigned int *frame,
                    unsigned char *subframe)
{

	// Fill in other TDD Configuration!!!!

	if (frame_parms->frame_type == FDD)
	{
		*subframe = current_subframe+6;

		if (*subframe>9)
		{
			*subframe = *subframe-10;
			*frame = (current_frame+1) & 1023;
		}
		else
		{
			*frame=current_frame;
		}
	}
	else     // TDD
	{
		if (frame_parms->tdd_config == 1)
		{
			switch (current_subframe)
			{

			case 0:
				*subframe = 7;
				*frame = current_frame;
				break;

			case 4:
				*subframe = 2;
				*frame = (current_frame+1) & 1023;
				break;

			case 5:
				*subframe = 2;
				*frame = (current_frame+1) & 1023;
				break;

			case 9:
				*subframe = 7;
				*frame = (current_frame+1) & 1023;
				break;
			}
		}
		else if (frame_parms->tdd_config == 3)
		{
			switch (current_subframe)
			{

			case 0:
			case 5:
			case 6:
				*subframe = 2;
				*frame = (current_frame+1) & 1023;
				break;

			case 7:
				*subframe = 3;
				*frame = (current_frame+1) & 1023;
				break;

			case 8:
				*subframe = 4;
				*frame = (current_frame+1) & 1023;
				break;

			case 9:
				*subframe = 2;
				*frame = (current_frame+2) & 1023;
				break;
			}
		}
		else if (frame_parms->tdd_config == 4)
		{
			switch (current_subframe)
			{

			case 0:
			case 4:
			case 5:
			case 6:
				*subframe = 2;
				*frame = (current_frame+1) & 1023;
				break;

			case 7:
				*subframe = 3;
				*frame = (current_frame+1) & 1023;
				break;

			case 8:
			case 9:
				*subframe = 2;
				*frame = (current_frame+2) & 1023;
				break;
			}
		}
		else if (frame_parms->tdd_config == 5)
		{
			switch (current_subframe)
			{

			case 0:
			case 4:
			case 5:
			case 6:
				*subframe = 2;
				*frame = (current_frame+1) & 1023;
				break;

			case 7:
			case 8:
			case 9:
				*subframe = 2;
				*frame = (current_frame+2) & 1023;
				break;
			}
		}
	}
}

void get_Msg3_alloc_ret(LTE_DL_FRAME_PARMS *frame_parms,
                        unsigned char current_subframe,
                        unsigned int current_frame,
                        unsigned int *frame,
                        unsigned char *subframe)
{
	if (frame_parms->frame_type == FDD)
	{
		/* always retransmit in n+8 */
		*subframe = current_subframe + 8;

		if (*subframe > 9)
		{
			*subframe = *subframe - 10;
			*frame = (current_frame + 1) & 1023;
		}
		else
		{
			*frame = current_frame;
		}
	}
	else
	{
		if (frame_parms->tdd_config == 1)
		{
			// original PUSCH in 2, PHICH in 6 (S), ret in 2
			// original PUSCH in 3, PHICH in 9, ret in 3
			// original PUSCH in 7, PHICH in 1 (S), ret in 7
			// original PUSCH in 8, PHICH in 4, ret in 8
			*frame = (current_frame+1) & 1023;
		}
		else if (frame_parms->tdd_config == 3)
		{
			// original PUSCH in 2, PHICH in 8, ret in 2 next frame
			// original PUSCH in 3, PHICH in 9, ret in 3 next frame
			// original PUSCH in 4, PHICH in 0, ret in 4 next frame
			*frame=(current_frame+1) & 1023;
		}
		else if (frame_parms->tdd_config == 4)
		{
			// original PUSCH in 2, PHICH in 8, ret in 2 next frame
			// original PUSCH in 3, PHICH in 9, ret in 3 next frame
			*frame=(current_frame+1) & 1023;
		}
		else if (frame_parms->tdd_config == 5)
		{
			// original PUSCH in 2, PHICH in 8, ret in 2 next frame
			*frame=(current_frame+1) & 1023;
		}
	}
}

uint8_t get_Msg3_harq_pid(LTE_DL_FRAME_PARMS *frame_parms,
                          uint32_t frame,
                          unsigned char current_subframe)
{

	uint8_t ul_subframe=0;
	uint32_t ul_frame=0;

	if (frame_parms->frame_type ==FDD)
	{
		ul_subframe = (current_subframe>3) ? (current_subframe-4) : (current_subframe+6);
		ul_frame    = (current_subframe>3) ? ((frame+1)&1023) : frame;
	}
	else
	{
		switch (frame_parms->tdd_config)
		{
		case 1:
			switch (current_subframe)
			{

			case 9:
			case 0:
				ul_subframe = 7;
				break;

			case 5:
			case 7:
				ul_subframe = 2;
				break;

			}

			break;

		// tdd_config  tdd_config_S
		case 2:
			switch(current_subframe)
			{
				case 0:
				case 5:
				case 6:
					ul_subframe = 2;
					break;
				case 7:
					ul_subframe = 3;
					break;
				case 8:
					ul_subframe = 4;
					break;
				case 9:
					ul_subframe = 2;
					break;
			}
			break;
			
		case 3:
			switch (current_subframe)
			{

			case 0:
			case 5:
			case 6:
				ul_subframe = 2;
				break;

			case 7:
				ul_subframe = 3;
				break;

			case 8:
				ul_subframe = 4;
				break;

			case 9:
				ul_subframe = 2;
				break;
			}

			break;

		case 4:
			switch (current_subframe)
			{

			case 0:
			case 5:
			case 6:
			case 8:
			case 9:
				ul_subframe = 2;
				break;

			case 7:
				ul_subframe = 3;
				break;
			}

			break;

		case 5:
			ul_subframe =2;
			break;

		default:
			LOG_E(PHY,"get_Msg3_harq_pid: Unsupported TDD configuration %d\n",frame_parms->tdd_config);
			mac_xface->macphy_exit("get_Msg3_harq_pid: Unsupported TDD configuration");
			break;
		}
	}

	return(subframe2harq_pid(frame_parms,ul_frame,ul_subframe));

}

unsigned char ul_ACK_subframe2_dl_subframe(LTE_DL_FRAME_PARMS *frame_parms,unsigned char subframe,unsigned char ACK_index)
{

	if (frame_parms->frame_type == FDD)
	{
		return((subframe<4) ? subframe+6 : subframe-4);
	}
	else
	{
		switch (frame_parms->tdd_config)
		{
		case 3:
			if (subframe == 2)    // ACK subframes 5 and 6
			{
				if (ACK_index==2)
					return(1);

				return(5+ACK_index);
			}
			else if (subframe == 3)     // ACK subframes 7 and 8
			{
				return(7+ACK_index);  // To be updated
			}
			else if (subframe == 4)     // ACK subframes 9 and 0
			{
				return((9+ACK_index)%10);
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/subframe2_dl_harq_pid: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;

		case 4:
			if (subframe == 2)    // ACK subframes 0, 4 and 5
			{
				//if (ACK_index==2)
				//  return(1); TBC
				if (ACK_index==2)
					return(0);

				return(4+ACK_index);
			}
			else if (subframe == 3)     // ACK subframes 6, 7 8 and 9
			{
				return(6+ACK_index);  // To be updated
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/subframe2_dl_harq_pid: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;

		case 1:
			if (subframe == 2)    // ACK subframes 5 and 6
			{
				return(5+ACK_index);
			}
			else if (subframe == 3)     // ACK subframe 9
			{
				return(9);  // To be updated
			}
			else if (subframe == 7)     // ACK subframes 0 and 1
			{
				return(ACK_index);  // To be updated
			}
			else if (subframe == 8)     // ACK subframe 4
			{
				return(4);  // To be updated
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/ul_ACK_subframe2_dl_subframe: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;
		}
	}

	return(0);
}

unsigned char ul_ACK_subframe2_M(LTE_DL_FRAME_PARMS *frame_parms,unsigned char subframe)
{

	if (frame_parms->frame_type == FDD)
	{
		return(1);
	}
	else
	{
		switch (frame_parms->tdd_config)
		{
		case 3:
			if (subframe == 2)    // ACK subframes 5 and 6
			{
				return(2); // should be 3
			}
			else if (subframe == 3)     // ACK subframes 7 and 8
			{
				return(2);  // To be updated
			}
			else if (subframe == 4)     // ACK subframes 9 and 0
			{
				return(2);
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/subframe2_dl_harq_pid: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;

		case 4:
			if (subframe == 2)    // ACK subframes 0,4 and 5
			{
				return(3); // should be 4
			}
			else if (subframe == 3)     // ACK subframes 6,7,8 and 9
			{
				return(4);
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/subframe2_dl_harq_pid: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;

		case 5:
			if (subframe == 2)    // ACK subframes 0,3,4,5,6,7,8 and 9
			{
				return(8); // should be 3
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/subframe2_dl_harq_pid: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;

		case 1:
			if (subframe == 2)    // ACK subframes 5 and 6
			{
				return(2);
			}
			else if (subframe == 3)     // ACK subframe 9
			{
				return(1);  // To be updated
			}
			else if (subframe == 7)     // ACK subframes 0 and 1
			{
				return(2);  // To be updated
			}
			else if (subframe == 8)     // ACK subframe 4
			{
				return(1);  // To be updated
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte_common.c/subframe2_dl_harq_pid: illegal subframe %d for tdd_config %d\n",
				      subframe,frame_parms->tdd_config);
				return(0);
			}

			break;
		}
	}

	return(0);
}

// This function implements table 10.1-1 of 36-213, p. 69
// return the number 'Nbundled'
uint8_t get_reset_ack(LTE_DL_FRAME_PARMS *frame_parms,
                      harq_status_t *harq_ack,
                      unsigned char subframe_tx,
                      unsigned char subframe_rx,
                      unsigned char *o_ACK,
                      uint8_t *pN_bundled,
                      uint8_t cw_idx,
                      uint8_t do_reset) // 1 to reset ACK/NACK status : 0 otherwise
{
	uint8_t status=0;
	uint8_t subframe_ul=0xff, subframe_dl0=0xff, subframe_dl1=0xff,subframe_dl2=0xff, subframe_dl3=0xff;

	//  printf("get_ack: SF %d\n",subframe);
	if (frame_parms->frame_type == FDD)
	{
		if (subframe_tx < 4)
			subframe_dl0 = subframe_tx + 6;
		else
			subframe_dl0 = subframe_tx - 4;

		o_ACK[cw_idx] = harq_ack[subframe_dl0].ack;
		status = harq_ack[subframe_dl0].send_harq_status;

		//LOG_I(PHY,"dl subframe %d send_harq_status %d cw_idx %d, reset %d\n",subframe_dl0, status, cw_idx, do_reset);
		if(do_reset)
			harq_ack[subframe_dl0].send_harq_status = 0;
		//printf("get_ack: Getting ACK/NAK for PDSCH (subframe %d) => %d\n",subframe_dl,o_ACK[0]);
	}
	else
	{
		switch (frame_parms->tdd_config)
		{
		case 1:
			if (subframe_tx == 2)    // ACK subframes 5,6
			{
				subframe_ul  = 6;
				subframe_dl0 = 5;
				subframe_dl1 = 6;
			}
			else if (subframe_tx == 3)     // ACK subframe 9
			{
				subframe_ul  = 9;
				subframe_dl0 = 9;
				subframe_dl1 = 0xff;
			}
			else if (subframe_tx == 4)     // nothing
			{
				subframe_ul  = 0xff;
				subframe_dl0 = 0xff; // invalid subframe number indicates ACK/NACK is not needed
				subframe_dl1 = 0xff;
			}
			else if (subframe_tx == 7)     // ACK subframes 0,1
			{
				subframe_ul  = 1;
				subframe_dl0 = 0;
				subframe_dl1 = 1;
			}
			else if (subframe_tx == 8)     // ACK subframes 4
			{
				subframe_ul  = 4;
				subframe_dl0 = 4;
				subframe_dl1 = 0xff;
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte.c: get_ack, illegal subframe_tx %d for tdd_config %d\n",
				      subframe_tx,frame_parms->tdd_config);
				return(0);
			}

			// report ACK/NACK status
			o_ACK[cw_idx] = 1;
			status = 0;
			if ((subframe_dl0 < 10) && (harq_ack[subframe_dl0].send_harq_status))
			{
				o_ACK[cw_idx] &= harq_ack[subframe_dl0].ack;
				status = harq_ack[subframe_dl0].send_harq_status;
			}
			if ((subframe_dl1 < 10) && (harq_ack[subframe_dl1].send_harq_status))
			{
				o_ACK[cw_idx] &= harq_ack[subframe_dl1].ack;
				status = harq_ack[subframe_dl1].send_harq_status;
			}
			// report status = Nbundled
			if (!status)
			{
				o_ACK[cw_idx] = 0;
			}
			else
			{
				if (harq_ack[subframe_ul].vDAI_UL < 0xff)
				{
					status = harq_ack[subframe_ul].vDAI_UL;
				}
			}

			if (!do_reset && (subframe_ul < 10))
			{
				if ((subframe_dl0 < 10) && (subframe_dl1 < 10))
				{
					LOG_D(PHY,"ul-sf#%d vDAI_UL[sf#%d]=%d Nbundled=%d: dlsf#%d ACK=%d harq_status=%d vDAI_DL=%d, dlsf#%d ACK=%d harq_status=%d vDAI_DL=%d, o_ACK[0]=%d status=%d\n",
					      subframe_tx, subframe_ul, harq_ack[subframe_ul].vDAI_UL, status,
					      subframe_dl0, harq_ack[subframe_dl0].ack, harq_ack[subframe_dl0].send_harq_status, harq_ack[subframe_dl0].vDAI_DL,
					      subframe_dl1, harq_ack[subframe_dl1].ack, harq_ack[subframe_dl1].send_harq_status, harq_ack[subframe_dl1].vDAI_DL,
					      o_ACK[cw_idx], status);
				}
				else if (subframe_dl0 < 10)
				{
					LOG_D(PHY,"ul-sf#%d vDAI_UL[sf#%d]=%d Nbundled=%d: dlsf#%d ACK=%d status=%d vDAI_DL=%d, o_ACK[0]=%d status=%d\n",
					      subframe_tx, subframe_ul, harq_ack[subframe_ul].vDAI_UL, status,
					      subframe_dl0, harq_ack[subframe_dl0].ack, harq_ack[subframe_dl0].send_harq_status, harq_ack[subframe_dl0].vDAI_DL,
					      o_ACK[cw_idx], status);
				}
				else if (subframe_dl1 < 10)
				{
					LOG_D(PHY,"ul-sf#%d vDAI_UL[sf#%d]=%d Nbundled=%d: dlsf#%d ACK=%d status=%d vDAI_DL=%d, o_ACK[0]=%d status=%d\n",
					      subframe_tx, subframe_ul, harq_ack[subframe_ul].vDAI_UL, status,
					      subframe_dl1, harq_ack[subframe_dl1].ack, harq_ack[subframe_dl1].send_harq_status, harq_ack[subframe_dl1].vDAI_DL,
					      o_ACK[cw_idx], status);
				}
			}

			// reset ACK/NACK status
			if (do_reset)
			{
				LOG_D(PHY,"ul-sf#%d ACK/NACK status resetting @ dci0-sf#%d, dci1x/2x-sf#%d, dci1x/2x-sf#%d\n", subframe_tx, subframe_ul, subframe_dl0, subframe_dl1);
				if (subframe_ul < 10)
				{
					harq_ack[subframe_ul].vDAI_UL = 0xff;
				}
				if (subframe_dl0 < 10)
				{
					harq_ack[subframe_dl0].vDAI_DL = 0xff;
					harq_ack[subframe_dl0].ack = 2;
					harq_ack[subframe_dl0].send_harq_status = 0;
				}
				if (subframe_dl1 < 10)
				{
					harq_ack[subframe_dl1].vDAI_DL = 0xff;
					harq_ack[subframe_dl1].ack = 2;
					harq_ack[subframe_dl1].send_harq_status = 0;
				}
			}

			break;

		case 3:
			if (subframe_tx == 2)    // ACK subframes 5 and 6
			{
				subframe_dl0 = 5;
				subframe_dl1 = 6;
				subframe_ul  = 2;
				//printf("subframe_tx 2, TDD config 3: harq_ack[5] = %d (%d),harq_ack[6] = %d (%d)\n",harq_ack[5].ack,harq_ack[5].send_harq_status,harq_ack[6].ack,harq_ack[6].send_harq_status);
			}
			else if (subframe_tx == 3)     // ACK subframes 7 and 8
			{
				subframe_dl0 = 7;
				subframe_dl1 = 8;
				subframe_ul  = 3;
				//printf("Subframe 3, TDD config 3: harq_ack[7] = %d,harq_ack[8] = %d\n",harq_ack[7].ack,harq_ack[8].ack);
				//printf("status %d : o_ACK (%d,%d)\n", status,o_ACK[0],o_ACK[1]);
			}
			else if (subframe_tx == 4)     // ACK subframes 9 and 0
			{
				subframe_dl0 = 9;
				subframe_dl1 = 0;
				subframe_ul  = 4;
				//printf("Subframe 4, TDD config 3: harq_ack[9] = %d,harq_ack[0] = %d\n",harq_ack[9].ack,harq_ack[0].ack);
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte.c: get_ack, illegal subframe_tx %d for tdd_config %d\n",
				      subframe_tx,frame_parms->tdd_config);
				return(0);
			}

			// report ACK/NACK status
			o_ACK[cw_idx] = 0;
			if (harq_ack[subframe_dl0].send_harq_status == 1)
			{
				o_ACK[cw_idx] = harq_ack[subframe_dl0].ack;

				if (harq_ack[subframe_dl1].send_harq_status == 1)
					o_ACK[cw_idx] &= harq_ack[subframe_dl1].ack;
			}
			else if (harq_ack[subframe_dl1].send_harq_status == 1)
				o_ACK[cw_idx] = harq_ack[subframe_dl1].ack;

			pN_bundled[0] = harq_ack[subframe_rx].vDAI_UL;
			status = harq_ack[subframe_dl0].send_harq_status + harq_ack[subframe_dl1].send_harq_status;

			//LOG_D(PHY,"TDD Config3 UL Sfn %d, dl Sfn0 %d status %d o_Ack %d, dl Sfn1 %d status %d o_Ack %d subframe_rx %d N_bundled %d \n",
			//	  subframe_tx, subframe_dl0, harq_ack[subframe_dl0].send_harq_status,harq_ack[subframe_dl0].ack,
			//    subframe_dl1, harq_ack[subframe_dl1].send_harq_status,harq_ack[subframe_dl1].ack, subframe_rx, pN_bundled[0]);
			if (do_reset)
			{
				// reset ACK/NACK status
				harq_ack[subframe_dl0].ack = 2;
				harq_ack[subframe_dl1].ack = 2;
				harq_ack[subframe_dl0].send_harq_status = 0;
				harq_ack[subframe_dl1].send_harq_status = 0;
			}

			break;

		case 4:
			if (subframe_tx == 2)    // ACK subframes 4, 5 and 0
			{
				subframe_dl0 = 0;
				subframe_dl1 = 4;
				subframe_dl2 = 5;
				subframe_ul  = 2;
				//printf("subframe_tx 2, TDD config 3: harq_ack[5] = %d (%d),harq_ack[6] = %d (%d)\n",harq_ack[5].ack,harq_ack[5].send_harq_status,harq_ack[6].ack,harq_ack[6].send_harq_status);
			}
			else if (subframe_tx == 3)     // ACK subframes 6, 7 8 and 9
			{
				subframe_dl0 = 7;
				subframe_dl1 = 8;
				subframe_dl2 = 9;
				subframe_dl3 = 6;
				subframe_ul  = 3;
				//printf("Subframe 3, TDD config 3: harq_ack[7] = %d,harq_ack[8] = %d\n",harq_ack[7].ack,harq_ack[8].ack);
				//printf("status %d : o_ACK (%d,%d)\n", status,o_ACK[0],o_ACK[1]);
			}
			else
			{
				LOG_E(PHY,"phy_procedures_lte.c: get_ack, illegal subframe_tx %d for tdd_config %d\n",
				      subframe_tx,frame_parms->tdd_config);
				return(0);
			}

			// report ACK/NACK status
			o_ACK[cw_idx] = 0;
			if (harq_ack[subframe_dl0].send_harq_status == 1)
				o_ACK[cw_idx] = harq_ack[subframe_dl0].ack;

			if (harq_ack[subframe_dl1].send_harq_status == 1)
				o_ACK[cw_idx] &= harq_ack[subframe_dl1].ack;

			if (harq_ack[subframe_dl2].send_harq_status == 1)
				o_ACK[cw_idx] &= harq_ack[subframe_dl2].ack;

			if (harq_ack[subframe_dl3].send_harq_status == 1)
				o_ACK[cw_idx] &= harq_ack[subframe_dl3].ack;

			pN_bundled[0] = harq_ack[subframe_rx].vDAI_UL;
			status = harq_ack[subframe_dl0].send_harq_status + harq_ack[subframe_dl1].send_harq_status + harq_ack[subframe_dl2].send_harq_status + harq_ack[subframe_dl3].send_harq_status;

			LOG_I(PHY,"TDD Config3 UL Sfn %d, dl Sfn0 %d status %d o_Ack %d, dl Sfn1 %d status %d o_Ack %d dl Sfn2 %d status %d o_Ack %d dl Sfn3 %d status %d o_Ack %d subframe_rx %d N_bundled %d status %d\n",
			      subframe_tx, subframe_dl0, harq_ack[subframe_dl0].send_harq_status,harq_ack[subframe_dl0].ack,
			      subframe_dl1, harq_ack[subframe_dl1].send_harq_status,harq_ack[subframe_dl1].ack,
			      subframe_dl2, harq_ack[subframe_dl2].send_harq_status,harq_ack[subframe_dl2].ack,
			      subframe_dl3, harq_ack[subframe_dl3].send_harq_status,harq_ack[subframe_dl3].ack,subframe_rx, pN_bundled[0], status);
			if (do_reset)
			{
				// reset ACK/NACK status
				harq_ack[subframe_dl0].ack = 2;
				harq_ack[subframe_dl1].ack = 2;
				harq_ack[subframe_dl2].ack = 2;
				harq_ack[subframe_dl3].ack = 2;
				harq_ack[subframe_dl0].send_harq_status = 0;
				harq_ack[subframe_dl1].send_harq_status = 0;
				harq_ack[subframe_dl2].send_harq_status = 0;
				harq_ack[subframe_dl3].send_harq_status = 0;
			}

			break;

		}
	}

	//printf("status %d\n",status);

	return(status);
}

uint8_t get_ack(LTE_DL_FRAME_PARMS *frame_parms,
                harq_status_t *harq_ack,
                unsigned char subframe_tx,
                unsigned char subframe_rx,
                unsigned char *o_ACK,
                uint8_t cw_idx)
{
	uint8_t N_bundled = 0;
	return get_reset_ack(frame_parms, harq_ack, subframe_tx, subframe_rx, o_ACK, &N_bundled, cw_idx, 0);
}

uint8_t reset_ack(LTE_DL_FRAME_PARMS *frame_parms,
                  harq_status_t *harq_ack,
                  unsigned char subframe_tx,
                  unsigned char subframe_rx,
                  unsigned char *o_ACK,
                  uint8_t *pN_bundled,
                  uint8_t cw_idx)
{
	return get_reset_ack(frame_parms, harq_ack, subframe_tx, subframe_rx, o_ACK, pN_bundled, cw_idx, 1);
}



uint8_t Np6[4]= {0,1,3,5};
uint8_t Np15[4]= {0,3,8,13};
uint8_t Np25[4]= {0,5,13,22};
uint8_t Np50[4]= {0,11,27,44};
uint8_t Np75[4]= {0,16,41,66};
uint8_t Np100[4]= {0,22,55,88};
// This is part of the PUCCH allocation procedure (see Section 10.1 36.213)
uint16_t get_Np(uint8_t N_RB_DL,uint8_t nCCE,uint8_t plus1)
{
	uint8_t *Np;

	switch (N_RB_DL)
	{
	case 6:
		Np=Np6;
		break;

	case 15:
		Np=Np15;
		break;

	case 25:
		Np=Np25;
		break;

	case 50:
		Np=Np50;
		break;

	case 75:
		Np=Np75;
		break;

	case 100:
		Np=Np100;
		break;

	default:
		LOG_E(PHY,"get_Np() FATAL: unsupported N_RB_DL %d\n",N_RB_DL);
		return(0);
		break;
	}

	if (nCCE>=Np[2])
		return(Np[2+plus1]);
	else if (nCCE>=Np[1])
		return(Np[1+plus1]);
	else
		return(Np[0+plus1]);
}

lte_subframe_t subframe_select(LTE_DL_FRAME_PARMS *frame_parms,unsigned char subframe)
{

	// if FDD return dummy value
	if (frame_parms->frame_type == FDD)
		return(SF_DL);

	switch (frame_parms->tdd_config)
	{

	case 1:
		switch (subframe)
		{
		case 0:
		case 4:
		case 5:
		case 9:
			return(SF_DL);
			break;

		case 2:
		case 3:
		case 7:
		case 8:
			return(SF_UL);
			break;

		default:
			return(SF_S);
			break;
		}

	case 2:
		switch(subframe)
		{
		case 0:
		case 3:
		case 4:
		case 5:
		case 8:
		case 9:
			return (SF_DL);
			break;

		case 2:
		case 7:
			return (SF_UL);
			break;
		default:
			return(SF_S);
			break;
		}

	case 3:
		if  ((subframe<1) || (subframe>=5))
			return(SF_DL);
		else if ((subframe>1) && (subframe < 5))
			return(SF_UL);
		else if (subframe==1)
			return (SF_S);
		else
		{
			LOG_E(PHY,"[PHY_PROCEDURES_LTE] Unknown subframe number\n");
			return(255);
		}

	case 4:
		if  ((subframe<1) || (subframe>=4))
			return(SF_DL);
		else if ((subframe>1) && (subframe < 4))
			return(SF_UL);
		else if (subframe==1)
			return (SF_S);
		else
		{
			LOG_E(PHY,"[PHY_PROCEDURES_LTE] Unknown subframe number\n");
			return(255);
		}

	case 5:
		if  ((subframe<1) || (subframe>=3))
			return(SF_DL);
		else if ((subframe>1) && (subframe < 3))
			return(SF_UL);
		else if (subframe==1)
			return (SF_S);
		else
		{
			LOG_E(PHY,"[PHY_PROCEDURES_LTE] Unknown subframe number\n");
			return(255);
		}
		break;

	default:
		LOG_E(PHY,"subframe %d Unsupported TDD configuration %d\n",subframe,frame_parms->tdd_config);
		mac_xface->macphy_exit("subframe x Unsupported TDD configuration");
		return(255);

	}
}

dci_detect_mode_t dci_detect_mode_select(LTE_DL_FRAME_PARMS *frame_parms,uint8_t subframe)
{
	dci_detect_mode_t ret = 0;

	static dci_detect_mode_t Table_8_2_3gpp_36_213[][10] =
	{
		//subf0    , subf1     , subf2 , subf3         , subf4     , subf5     , subf6     , subf7 , subf8     , subf9
		{UL_DL_DCI, UL_DL_DCI , NO_DCI    , NO_DCI    , NO_DCI    , UL_DL_DCI , UL_DL_DCI , NO_DCI, NO_DCI    , NO_DCI    },  // tdd0
		{DL_DCI   , UL_DL_DCI , NO_DCI    , NO_DCI    , UL_DL_DCI , DL_DCI    , UL_DL_DCI , NO_DCI, NO_DCI    , UL_DL_DCI },  // tdd1
		{DL_DCI   , DL_DCI    , NO_DCI    , UL_DL_DCI , DL_DCI    , DL_DCI    , DL_DCI    , NO_DCI, UL_DL_DCI , DL_DCI    },  // tdd2
		{UL_DL_DCI, DL_DCI    , NO_DCI    , NO_DCI    , NO_DCI    , DL_DCI    , DL_DCI    , DL_DCI, UL_DL_DCI , UL_DL_DCI },  // tdd3
		{DL_DCI   , DL_DCI    , NO_DCI    , NO_DCI    , DL_DCI    , DL_DCI    , DL_DCI    , DL_DCI, UL_DL_DCI , UL_DL_DCI },  // tdd4
		{DL_DCI   , DL_DCI    , NO_DCI    , DL_DCI    , DL_DCI    , DL_DCI    , DL_DCI    , DL_DCI, UL_DL_DCI , DL_DCI    },  // tdd5
		{UL_DL_DCI, UL_DL_DCI , NO_DCI    , NO_DCI    , NO_DCI    , UL_DL_DCI , UL_DL_DCI , NO_DCI, NO_DCI    , UL_DL_DCI }
	}; // tdd6


	DevAssert(subframe>=0 && subframe<=9);
	DevAssert((frame_parms->tdd_config)>=0 && (frame_parms->tdd_config)<=6);

	if (frame_parms->frame_type == FDD)
	{
		ret = UL_DL_DCI;
	}
	else
	{
		ret = Table_8_2_3gpp_36_213[frame_parms->tdd_config][subframe];
	}

	LOG_D(PHY, "subframe %d: detect UL_DCI=%d, detect DL_DCI=%d\n", subframe, (ret & UL_DCI)>0, (ret & DL_DCI)>0);
	return ret;
}

lte_subframe_t get_subframe_direction(uint8_t Mod_id,uint8_t CC_id,uint8_t subframe)
{

	return(subframe_select(&PHY_vars_eNB_g[Mod_id][CC_id]->frame_parms,subframe));

}

uint8_t phich_subframe_to_harq_pid(LTE_DL_FRAME_PARMS *frame_parms,uint32_t frame,uint8_t subframe)
{

	//LOG_D(PHY,"phich_subframe_to_harq_pid.c: frame %d, subframe %d\n",frame,subframe);
	return(subframe2harq_pid(frame_parms,
	                         phich_frame2_pusch_frame(frame_parms,frame,subframe),
	                         phich_subframe2_pusch_subframe(frame_parms,subframe)));
}

unsigned int is_phich_subframe(LTE_DL_FRAME_PARMS *frame_parms,unsigned char subframe)
{

	if (frame_parms->frame_type == FDD)
	{
		return(1);
	}
	else
	{
		switch (frame_parms->tdd_config)
		{
		case 1:
			if ((subframe == 1) || (subframe == 4) || (subframe == 6) || (subframe == 9))
				return(1);

			break;

		case 3:
			if ((subframe == 0) || (subframe == 8) || (subframe == 9))
				return(1);

			break;

		case 4:
			if ((subframe == 8) || (subframe == 9) )
				return(1);

			break;

		case 5:
			if (subframe == 8)
				return(1);

			break;

		default:
			return(0);
			break;
		}
	}

	return(0);
}


#ifdef LOCALIZATION
double aggregate_eNB_UE_localization_stats(PHY_VARS_eNB *phy_vars_eNB, int8_t UE_id, frame_t frame, sub_frame_t subframe, int32_t UE_tx_power_dB)
{
	// parameters declaration
	int8_t Mod_id, CC_id;
	//    int32_t harq_pid;
	int32_t avg_power, avg_rssi, median_power, median_rssi, median_subcarrier_rss, median_TA, median_TA_update, ref_timestamp_ms, current_timestamp_ms;
	char cqis[100], sub_powers[2048];
	int len = 0, i;
	struct timeval ts;
	double sys_bw = 0;
	uint8_t N_RB_DL;
	LTE_DL_FRAME_PARMS *frame_parms = &eNB->frame_parms;

	Mod_id = eNB->Mod_id;
	CC_id = eNB->CC_id;
	ref_timestamp_ms = eNB->ulsch[UE_id+1]->reference_timestamp_ms;

	for (i=0; i<13; i++)
	{
		len += sprintf(&cqis[len]," %d ", eNB->UE_stats[(uint32_t)UE_id].DL_subband_cqi[0][i]);
	}

	len = 0;

	for (i=0; i<eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->active_subcarrier; i++)
	{
		len += sprintf(&sub_powers[len]," %d ", eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->subcarrier_power[i]);
	}

	gettimeofday(&ts, NULL);
	current_timestamp_ms = ts.tv_sec * 1000 + ts.tv_usec / 1000;


	LOG_D(LOCALIZE, " PHY: [UE %x/%d -> eNB %d], timestamp %d, "
	      "frame %d, subframe %d"
	      "UE Tx power %d dBm, "
	      "RSSI ant1 %d dBm, "
	      "RSSI ant2 %d dBm, "
	      "pwr ant1 %d dBm, "
	      "pwr ant2 %d dBm, "
	      "Rx gain %d dB, "
	      "TA %d, "
	      "TA update %d, "
	      "DL_CQI (%d,%d), "
	      "Wideband CQI (%d,%d), "
	      "DL Subband CQI[13] %s \n",
	      //          "timestamp %d, (%d active subcarrier) %s \n"
	      eNB->dlsch[(uint32_t)UE_id][0]->rnti, UE_id, Mod_id, current_timestamp_ms,
	      frame,subframe,
	      UE_tx_power_dB,
	      eNB->UE_stats[(uint32_t)UE_id].UL_rssi[0],
	      eNB->UE_stats[(uint32_t)UE_id].UL_rssi[1],
	      dB_fixed(eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->ulsch_power[0]),
	      dB_fixed(eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->ulsch_power[1]),
	      eNB->rx_total_gain_eNB_dB,
	      eNB->UE_stats[(uint32_t)UE_id].UE_timing_offset, // raw timing advance 1/sampling rate
	      eNB->UE_stats[(uint32_t)UE_id].timing_advance_update,
	      eNB->UE_stats[(uint32_t)UE_id].DL_cqi[0],eNB->UE_stats[(uint32_t)UE_id].DL_cqi[1],
	      eNB->measurements[Mod_id].wideband_cqi_dB[(uint32_t)UE_id][0],
	      eNB->measurements[Mod_id].wideband_cqi_dB[(uint32_t)UE_id][1],
	      cqis);
	LOG_D(LOCALIZE, " PHY: timestamp %d, (%d active subcarrier) %s \n",
	      current_timestamp_ms,
	      eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->active_subcarrier,
	      sub_powers);

	N_RB_DL = frame_parms->N_RB_DL;

	switch (N_RB_DL)
	{
	case 6:
		sys_bw = 1.92;
		break;

	case 25:
		sys_bw = 7.68;
		break;

	case 50:
		sys_bw = 15.36;
		break;

	case 100:
		sys_bw = 30.72;
		break;
	}

	if ((current_timestamp_ms - ref_timestamp_ms > eNB->ulsch[UE_id+1]->aggregation_period_ms))
	{
		// check the size of one list to be sure there was a message transmitted during the defined aggregation period

		// make the reference timestamp == current timestamp
		eNB->ulsch[UE_id+1]->reference_timestamp_ms = current_timestamp_ms;
		int i;

		for (i=0; i<10; i++)
		{
			median_power = calculate_median(&eNB->ulsch[UE_id+1]->loc_rss_list[i]);
			del(&eNB->ulsch[UE_id+1]->loc_rss_list[i]);
			median_rssi = calculate_median(&eNB->ulsch[UE_id+1]->loc_rssi_list[i]);
			del(&eNB->ulsch[UE_id+1]->loc_rssi_list[i]);
			median_subcarrier_rss = calculate_median(&eNB->ulsch[UE_id+1]->loc_subcarrier_rss_list[i]);
			del(&eNB->ulsch[UE_id+1]->loc_subcarrier_rss_list[i]);
			median_TA = calculate_median(&eNB->ulsch[UE_id+1]->loc_timing_advance_list[i]);
			del(&eNB->ulsch[UE_id+1]->loc_timing_advance_list[i]);
			median_TA_update = calculate_median(&eNB->ulsch[UE_id+1]->loc_timing_update_list[i]);
			del(&eNB->ulsch[UE_id+1]->loc_timing_update_list[i]);

			if (median_power != 0)
				push_front(&eNB->ulsch[UE_id+1]->tot_loc_rss_list,median_power);

			if (median_rssi != 0)
				push_front(&eNB->ulsch[UE_id+1]->tot_loc_rssi_list,median_rssi);

			if (median_subcarrier_rss != 0)
				push_front(&eNB->ulsch[UE_id+1]->tot_loc_subcarrier_rss_list,median_subcarrier_rss);

			if (median_TA != 0)
				push_front(&eNB->ulsch[UE_id+1]->tot_loc_timing_advance_list,median_TA);

			if (median_TA_update != 0)
				push_front(&eNB->ulsch[UE_id+1]->tot_loc_timing_update_list,median_TA_update);

			initialize(&eNB->ulsch[UE_id+1]->loc_rss_list[i]);
			initialize(&eNB->ulsch[UE_id+1]->loc_subcarrier_rss_list[i]);
			initialize(&eNB->ulsch[UE_id+1]->loc_rssi_list[i]);
			initialize(&eNB->ulsch[UE_id+1]->loc_timing_advance_list[i]);
			initialize(&eNB->ulsch[UE_id+1]->loc_timing_update_list[i]);
		}

		median_power = calculate_median(&eNB->ulsch[UE_id+1]->tot_loc_rss_list);
		del(&eNB->ulsch[UE_id+1]->tot_loc_rss_list);
		median_rssi = calculate_median(&eNB->ulsch[UE_id+1]->tot_loc_rssi_list);
		del(&eNB->ulsch[UE_id+1]->tot_loc_rssi_list);
		median_subcarrier_rss = calculate_median(&eNB->ulsch[UE_id+1]->tot_loc_subcarrier_rss_list);
		del(&eNB->ulsch[UE_id+1]->tot_loc_subcarrier_rss_list);
		median_TA = calculate_median(&eNB->ulsch[UE_id+1]->tot_loc_timing_advance_list);
		del(&eNB->ulsch[UE_id+1]->tot_loc_timing_advance_list);
		median_TA_update = calculate_median(&eNB->ulsch[UE_id+1]->tot_loc_timing_update_list);
		del(&eNB->ulsch[UE_id+1]->tot_loc_timing_update_list);

		initialize(&eNB->ulsch[UE_id+1]->tot_loc_rss_list);
		initialize(&eNB->ulsch[UE_id+1]->tot_loc_subcarrier_rss_list);
		initialize(&eNB->ulsch[UE_id+1]->tot_loc_rssi_list);
		initialize(&eNB->ulsch[UE_id+1]->tot_loc_timing_advance_list);
		initialize(&eNB->ulsch[UE_id+1]->tot_loc_timing_update_list);

		double alpha = 2, power_distance, time_distance;
		// distance = 10^((Ptx - Prx - A)/10alpha), A is a constance experimentally evaluated
		// A includes the rx gain (eNB->rx_total_gain_eNB_dB) and hardware calibration
		power_distance = pow(10, ((UE_tx_power_dB - median_power - eNB->rx_total_gain_eNB_dB + 133)/(10.0*alpha)));
		/* current measurements shows constant UE_timing_offset = 18
		   and timing_advance_update = 11 at 1m. at 5m, timing_advance_update = 12*/
		//time_distance = (double) 299792458*(eNB->UE_stats[(uint32_t)UE_id].timing_advance_update)/(sys_bw*1000000);
		time_distance = (double) abs(eNB->UE_stats[(uint32_t)UE_id].timing_advance_update - 11) * 4.89;//  (3 x 108 x 1 / (15000 x 2048)) / 2 = 4.89 m

		eNB->UE_stats[(uint32_t)UE_id].distance.time_based = time_distance;
		eNB->UE_stats[(uint32_t)UE_id].distance.power_based = power_distance;

		LOG_D(LOCALIZE, " PHY agg [UE %x/%d -> eNB %d], timestamp %d, "
		      "frame %d, subframe %d "
		      "UE Tx power %d dBm, "
		      "median RSSI %d dBm, "
		      "median Power %d dBm, "
		      "Rx gain %d dB, "
		      "power estimated r = %0.3f, "
		      " TA %d, update %d "
		      "TA estimated r = %0.3f\n"
		      ,eNB->dlsch[(uint32_t)UE_id][0]->rnti, UE_id, Mod_id, current_timestamp_ms,
		      frame, subframe,
		      UE_tx_power_dB,
		      median_rssi,
		      median_power,
		      eNB->rx_total_gain_eNB_dB,
		      power_distance,
		      eNB->UE_stats[(uint32_t)UE_id].UE_timing_offset, median_TA_update,
		      time_distance);

		return 0;
	}
	else
	{
		avg_power = (dB_fixed(eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->ulsch_power[0]) + dB_fixed(eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->ulsch_power[1]))/2;
		avg_rssi = (eNB->UE_stats[(uint32_t)UE_id].UL_rssi[0] + eNB->UE_stats[(uint32_t)UE_id].UL_rssi[1])/2;

		push_front(&eNB->ulsch[UE_id+1]->loc_rss_list[subframe],avg_power);
		push_front(&eNB->ulsch[UE_id+1]->loc_rssi_list[subframe],avg_rssi);

		for (i=0; i<eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->active_subcarrier; i++)
		{
			push_front(&eNB->ulsch[UE_id+1]->loc_subcarrier_rss_list[subframe], eNB->lte_eNB_pusch_vars[(uint32_t)UE_id]->subcarrier_power[i]);
		}

		push_front(&eNB->ulsch[UE_id+1]->loc_timing_advance_list[subframe], eNB->UE_stats[(uint32_t)UE_id].UE_timing_offset);
		push_front(&eNB->ulsch[UE_id+1]->loc_timing_update_list[subframe], eNB->UE_stats[(uint32_t)UE_id].timing_advance_update);
		return -1;
	}
}
#endif
LTE_eNB_UE_stats* get_UE_stats(uint8_t Mod_id, uint8_t  CC_id,uint16_t rnti)
{
	int8_t UE_id;

	if ((PHY_vars_eNB_g == NULL) || (PHY_vars_eNB_g[Mod_id] == NULL) || (PHY_vars_eNB_g[Mod_id][CC_id]==NULL))
	{
		LOG_E(PHY,"get_UE_stats: No eNB found (or not allocated) for Mod_id %d,CC_id %d\n",Mod_id,CC_id);
		return NULL;
	}

	UE_id = find_ue(rnti, PHY_vars_eNB_g[Mod_id][CC_id]);

	if (UE_id == -1)
	{
		//    LOG_E(PHY,"get_UE_stats: UE with rnti %x not found\n",rnti);
		return NULL;
	}

	return(&PHY_vars_eNB_g[Mod_id][CC_id]->UE_stats[(uint32_t)UE_id]);
}

int8_t find_ue(uint16_t rnti, PHY_VARS_eNB *eNB)
{
	uint8_t i;

	for (i=0; i<NUMBER_OF_UE_MAX; i++)
	{
		if ((eNB->dlsch[i]) &&
		        (eNB->dlsch[i][0]) &&
		        (eNB->dlsch[i][0]->rnti==rnti))
		{
			return(i);
		}
	}

#ifdef CBA

	for (i=0; i<NUM_MAX_CBA_GROUP; i++)
	{
		if ((eNB->ulsch[i]) && // ue J is the representative of group j
		        (eNB->ulsch[i]->num_active_cba_groups) &&
		        (eNB->ulsch[i]->cba_rnti[i]== rnti))
			return(i);
	}

#endif

	return(-1);
}

LTE_DL_FRAME_PARMS* get_lte_frame_parms(module_id_t Mod_id, uint8_t  CC_id)
{

	return(&PHY_vars_eNB_g[Mod_id][CC_id]->frame_parms);

}

MU_MIMO_mode *get_mu_mimo_mode (module_id_t Mod_id, uint8_t  CC_id, rnti_t rnti)
{
	int8_t UE_id = find_ue( rnti, PHY_vars_eNB_g[Mod_id][CC_id] );

	if (UE_id == -1)
		return 0;

	return &PHY_vars_eNB_g[Mod_id][CC_id]->mu_mimo_mode[UE_id];
}

int is_srs_occasion_common(LTE_DL_FRAME_PARMS *frame_parms,int frame_tx,int subframe_tx)
{
	uint8_t isSubframeSRS   = 0; // SRS Cell Occasion

	//ue->ulsch[eNB_id]->srs_active   = 0;
	//ue->ulsch[eNB_id]->Nsymb_pusch  = 12-(frame_parms->Ncp<<1)- ue->ulsch[eNB_id]->srs_active;
	if(frame_parms->soundingrs_ul_config_common.enabled_flag)
	{

		LOG_D(PHY," SRS SUBFRAMECONFIG: %d\n", frame_parms->soundingrs_ul_config_common.srs_SubframeConfig);

		uint8_t  TSFC;
		uint16_t deltaTSFC; // bitmap
		uint8_t  srs_SubframeConfig;

		// table resuming TSFC (Period) and deltaSFC (offset)
		const uint16_t deltaTSFCTabType1[15][2] = { {1,1},{1,2},{2,2},{1,5},{2,5},{4,5},{8,5},{3,5},{12,5},{1,10},{2,10},{4,10},{8,10},{351,10},{383,10} };      // Table 5.5.3.3-2 3GPP 36.211 FDD
		const uint16_t deltaTSFCTabType2[14][2] = { {2,5},{6,5},{10,5},{18,5},{14,5},{22,5},{26,5},{30,5},{70,10},{74,10},{194,10},{326,10},{586,10},{210,10} }; // Table 5.5.3.3-2 3GPP 36.211 TDD

		srs_SubframeConfig = frame_parms->soundingrs_ul_config_common.srs_SubframeConfig;
		if (FDD == frame_parms->frame_type)
		{
			// srs_SubframeConfig =< 14
			deltaTSFC = deltaTSFCTabType1[srs_SubframeConfig][0];
			TSFC      = deltaTSFCTabType1[srs_SubframeConfig][1];
		}
		else
		{
			// srs_SubframeConfig =< 13
			deltaTSFC = deltaTSFCTabType2[srs_SubframeConfig][0];
			TSFC      = deltaTSFCTabType2[srs_SubframeConfig][1];
		}

		// Sounding reference signal subframes are the subframes satisfying ns/2 mod TSFC (- deltaTSFC
		uint16_t tmp = (subframe_tx %  TSFC);
		if((1<<tmp) & deltaTSFC)
		{
			// This is a Sounding reference signal subframes
			isSubframeSRS = 1;
		}
		LOG_D(PHY," ISTDD: %d, TSFC: %d, deltaTSFC: %d, AbsSubframeTX: %d.%d\n", frame_parms->frame_type, TSFC, deltaTSFC, frame_tx, subframe_tx);
	}
	return(isSubframeSRS);
}

void compute_srs_pos(lte_frame_type_t frameType,uint16_t isrs,uint16_t *psrsPeriodicity,uint16_t *psrsOffset)
{
	if(TDD == frameType)
	{
		if(isrs<10)
		{
			mac_xface->macphy_exit("2 ms SRS periodicity not supported");
		}

		if((isrs>9)&&(isrs<15))
		{
			*psrsPeriodicity=5;
			*psrsOffset=isrs-10;
		}
		if((isrs>14)&&(isrs<25))
		{
			*psrsPeriodicity=10;
			*psrsOffset=isrs-15;
		}
		if((isrs>24)&&(isrs<45))
		{
			*psrsPeriodicity=20;
			*psrsOffset=isrs-25;
		}
		if((isrs>44)&&(isrs<85))
		{
			*psrsPeriodicity=40;
			*psrsOffset=isrs-45;
		}
		if((isrs>84)&&(isrs<165))
		{
			*psrsPeriodicity=80;
			*psrsOffset=isrs-85;
		}
		if((isrs>164)&&(isrs<325))
		{
			*psrsPeriodicity=160;
			*psrsOffset=isrs-165;
		}
		if((isrs>324)&&(isrs<645))
		{
			*psrsPeriodicity=320;
			*psrsOffset=isrs-325;
		}

		if(isrs>644)
		{
			mac_xface->macphy_exit("Isrs out of range");
		}

	}
	else
	{
		if(isrs<2)
		{
			*psrsPeriodicity=2;
			*psrsOffset=isrs;
		}
		if((isrs>1)&&(isrs<7))
		{
			*psrsPeriodicity=5;
			*psrsOffset=isrs-2;
		}
		if((isrs>6)&&(isrs<17))
		{
			*psrsPeriodicity=10;
			*psrsOffset=isrs-7;
		}
		if((isrs>16)&&(isrs<37))
		{
			*psrsPeriodicity=20;
			*psrsOffset=isrs-17;
		}
		if((isrs>36)&&(isrs<77))
		{
			*psrsPeriodicity=40;
			*psrsOffset=isrs-37;
		}
		if((isrs>76)&&(isrs<157))
		{
			*psrsPeriodicity=80;
			*psrsOffset=isrs-77;
		}
		if((isrs>156)&&(isrs<317))
		{
			*psrsPeriodicity=160;
			*psrsOffset=isrs-157;
		}
		if((isrs>316)&&(isrs<637))
		{
			*psrsPeriodicity=320;
			*psrsOffset=isrs-317;
		}
		if(isrs>636)
		{
			mac_xface->macphy_exit("Isrs out of range");
		}
	}
}
