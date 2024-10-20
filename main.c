/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:48 by roaraujo          #+#    #+#             */
/*   Updated: 2024/06/03 23:17:44 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "ft_malloc.h"
#include "libft.h"

int	main(void)
{
	void *ptr1;
	void *ptr2;
	void *ptr3;
	// void *ptr4;
	// void *ptr5;
	// void *ptr6;
	// void *ptr7;
	// void *ptr8;
	// void *ptr9;
	// void *ptr10;
	// void *ptr11;

	const char *str1 = "Hello world!";
	ptr1 = malloc((ft_strlen(str1) + 1) * sizeof(char));
	ft_memcpy(ptr1, "Hello world!", ft_strlen(str1));
	((char *) ptr1)[ft_strlen(str1)] = '\0';

	const char *str2 = "And there was a booming above you; That night, black airplanes flew over the sea; And they were lowing and shifting like; Beached whales; Shelled snails; As you strained and you squinted to see; The retreat of their hairless and blind cavalry.";
	ptr2 = malloc((ft_strlen(str2) + 1) * sizeof(char));
	ft_memcpy(ptr2, str2, ft_strlen(str2));
	((char *) ptr2)[ft_strlen(str2)] = '\0';

	const char *str3 = "Down in the green hay\nWhere monkey and bear usually lay (lay)\nThey woke from a stable-boy's cry\nSaid someone come quick\nThe horses got loose, got grass-sick\nThey'll founder, fain, they'll die\n\nWhat is now known by the sorrel and the roan?\nBy the chestnut, and the bay, and the gelding grey?\nIt is, stay by the gate you are given\nAnd remain in your place, for your season\nAnd had the overfed dead but listened\nTo that high-fence, horse-sense, wisdom\n\nBut Did you hear that, Bear? said\nMonkey, we'll get out of here, fair and square\nThey left the gate open wide\n\nSo, my bride, here is my hand Where is your paw?\nTry and understand my plan, Ursula\nMy heart is a furnace\nFull of love that's just and earnest\nNow you know that we must unlearn this\nAllegiance to a life of service\nAnd no longer answer to that heartless\nHay-monger, nor be his accomplice\nThe charlatan, with artless hustling\nBut Ursula, we've got to eat something\nAnd earn our keep, while still within\nThe borders of the land that man has girded\nAll double-bolted and tightfisted\nUntil we reach the open country\nA-steeped in milk and honey\nWill you keep your fancy clothes on, for me?\nCan you bare a little longer to wear that leash?\n\nMy love, I swear by the air I breathe\nSooner or later, you'll bare your teeth\n\nBut for now, just dance, darling\nC'mon, will you dance, my darling?\nDarling, there's a place for us\nCan we go, before I turn to dust?\nOh, my darling there's a place for us\n\nOh darling, c'mon will you dance my darling?\nThough the hills are groaning with excess\nLike a table ceaselessly being set\nOh my darling, we will get there yet\n\nThey trooped past the guards\nPast the coops, and the fields\nAnd the farmyards, all night till finally\n\nThe space they gained grew much farther than\nThe stone that Bear threw\nTo mark where they'd stop for tea\n\nBut Walk a little faster, don't look backwards\nYour feast is to the East, which lies a little past the pasture\nAnd the blackbirds hear tea whistling they rise and clap\nAnd their applause caws the kettle black\nAnd we can't have none of that\nMove along, Bear, there, there, that's that\n\nThough cast in plaster\nOur Ursula's heart beat faster\nThan monkey's ever will\n\nBut still, they had got to pay the bills\nHadn't they? That is what the monkey'd say\nSo, with the courage of a clown, or a cur\nOr a kite, jerking tight at its tether\nIn her dung-brown gown of fur\nAnd her jerkin of swan's down and leather\nBear would sway on her hind legs\nThe organ would grind dregs of song\nFor the pleasure of the children who'd shriek\nThrowing coins at her feet and recoiling in terror\n\nSing, Dance, darling\nC'mon, will you dance, my darling?\nOh darling, there's a place for us\nCan we go, before I turn to dust?\nOh my darling there's a place for us\n\nOh darling, c'mon, will you dance, my darling?\nYou keep your eyes fixed on the highest hill\nWhere you'll ever-after eat your fill\nOh my darling dear mine, if you dance\nDance darling, and I'll love you still\n\nDeep in the night, shone a weak and miserly light\nWhere the monkey shouldered his lamp\nSomeone had told him the\nBear'd been wandering a fair piece away\nFrom where they were camped\nSomeone had told him the bear'd been sneaking away\nTo the seaside caverns, to bathe\nAnd the thought troubled the monkey\nFor he was afraid of spelunking\nDown in those caves, also afraid what the\nVillage people would say if they saw the bear in that state\nLolling and splashing obscenely\nWell, it seemed irrational, really\nWashing that face, washing that matted and flea-bit pelt\nIn some sea-spit-shine old kelp dripping with brine\nBut monkey just laughed, and he muttered\nWhen she comes back, Ursula will be bursting with pride\nTill I jump up saying, You've been rolling in muck\nSaying, You smell of garbage and grime\n\nBut far out, far out, by now, by now\nFar out, by now, Bear ploughed\n'Cause she would not drown\n\nFirst the outside-legs of the bear\nUp and fell down, in the water, like knobby garters\nThen the outside-arms of the bear\nFell off, as easy as if sloughed from boiled tomatoes\nLowered in a genteel curtsy\nBear shed the mantle of her diluvian shoulders\nAnd, with a sigh she allowed the burden of belly to drop\nLike an apron full of boulders\n\nIf you could hold up her threadbare coat to the light\nWhere it's worn translucent in places\nYou'd see spots where\nAlmost every night of the year\nBear had been mending, suspending that baseness\n\nNow her coat drags through the water\nBagging, with a life's-worth of hunger\nLimitless minnows\n\nIn the magnetic embrace, balletic and glacial\nOf bear's insatiable shadow\n\nLeft there, left there\nWhen Bear left Bear\n\nLeft there, left there\nWhen bear stepped clear of bear\n\nSooner or later you'll bury your teeth.";
	ptr3 = malloc((ft_strlen(str3) + 1) * sizeof(char));
	ft_memcpy(ptr3, str3, ft_strlen(str3));
	((char *) ptr3)[ft_strlen(str3)] = '\0';

	// ptr4 = malloc(23 * sizeof(char));
	// ptr5 = malloc(52 * sizeof(char));
	// ptr6 = malloc(643 * sizeof(char));
	// ptr7 = malloc(2500 * sizeof(char));
	// ptr8 = malloc(4096 * sizeof(char));
	// ptr9 = malloc(40960 * sizeof(char));
	// ptr10 = malloc(409600 * sizeof(char));
	// ptr11 = malloc(4096000 * sizeof(char));

	show_alloc_mem();
	show_hex_dump();

	free(ptr1 + 1);
	free(ptr2 + 1);
	free(ptr3 + 1);
	// free(ptr4 + 1);
	// free(ptr5 + 1);
	// free(ptr6 + 1);
	free(ptr1);
	free(ptr2);
	free(ptr3);
	// free(ptr4);
	// free(ptr5);
	// free(ptr6);
	// free(ptr7);
	// free(ptr8);
	// free(ptr9);
	// free(ptr10);
	// free(ptr11);

	show_alloc_mem();
	show_hex_dump();

	return (0);
}
