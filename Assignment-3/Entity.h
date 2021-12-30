#pragma once


/*
when loading animation onto entity, specifiy whether it's repeating or not
if not repeating, should destory entity after animation.hasEnded() returns true

example: hitting brick from below
it changes the entity's brick animation to explosion animation, and specifies
that it is not repeating. when the animation ends, destroy the entity
*/

class Entity
{
};

