impl :: bincode :: Encode for FidePlayer
{
    fn encode < __E : :: bincode :: enc :: Encoder >
    (& self, encoder : & mut __E) ->core :: result :: Result < (), :: bincode
    :: error :: EncodeError >
    {
        :: bincode :: Encode :: encode(&self.fideid, encoder) ?; :: bincode ::
        Encode :: encode(&self.name, encoder) ?; :: bincode :: Encode ::
        encode(&self.country, encoder) ?; :: bincode :: Encode ::
        encode(&self.sex, encoder) ?; :: bincode :: Encode ::
        encode(&self.title, encoder) ?; :: bincode :: Encode ::
        encode(&self.w_title, encoder) ?; :: bincode :: Encode ::
        encode(&self.o_title, encoder) ?; :: bincode :: Encode ::
        encode(&self.foa_title, encoder) ?; :: bincode :: Encode ::
        encode(&self.rating, encoder) ?; :: bincode :: Encode ::
        encode(&self.games, encoder) ?; :: bincode :: Encode ::
        encode(&self.k, encoder) ?; :: bincode :: Encode ::
        encode(&self.rapid_rating, encoder) ?; :: bincode :: Encode ::
        encode(&self.rapid_games, encoder) ?; :: bincode :: Encode ::
        encode(&self.rapid_k, encoder) ?; :: bincode :: Encode ::
        encode(&self.blitz_rating, encoder) ?; :: bincode :: Encode ::
        encode(&self.blitz_games, encoder) ?; :: bincode :: Encode ::
        encode(&self.blitz_k, encoder) ?; :: bincode :: Encode ::
        encode(&self.birthday, encoder) ?; :: bincode :: Encode ::
        encode(&self.flag, encoder) ?; Ok(())
    }
}