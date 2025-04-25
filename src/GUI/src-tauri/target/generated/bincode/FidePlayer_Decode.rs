impl :: bincode :: Decode for FidePlayer
{
    fn decode < __D : :: bincode :: de :: Decoder > (decoder : & mut __D)
    ->core :: result :: Result < Self, :: bincode :: error :: DecodeError >
    {
        Ok(Self
        {
            fideid : :: bincode :: Decode :: decode(decoder) ?, name : ::
            bincode :: Decode :: decode(decoder) ?, country : :: bincode ::
            Decode :: decode(decoder) ?, sex : :: bincode :: Decode ::
            decode(decoder) ?, title : :: bincode :: Decode :: decode(decoder)
            ?, w_title : :: bincode :: Decode :: decode(decoder) ?, o_title :
            :: bincode :: Decode :: decode(decoder) ?, foa_title : :: bincode
            :: Decode :: decode(decoder) ?, rating : :: bincode :: Decode ::
            decode(decoder) ?, games : :: bincode :: Decode :: decode(decoder)
            ?, k : :: bincode :: Decode :: decode(decoder) ?, rapid_rating :
            :: bincode :: Decode :: decode(decoder) ?, rapid_games : ::
            bincode :: Decode :: decode(decoder) ?, rapid_k : :: bincode ::
            Decode :: decode(decoder) ?, blitz_rating : :: bincode :: Decode
            :: decode(decoder) ?, blitz_games : :: bincode :: Decode ::
            decode(decoder) ?, blitz_k : :: bincode :: Decode ::
            decode(decoder) ?, birthday : :: bincode :: Decode ::
            decode(decoder) ?, flag : :: bincode :: Decode :: decode(decoder)
            ?,
        })
    }
} impl < '__de > :: bincode :: BorrowDecode < '__de > for FidePlayer
{
    fn borrow_decode < __D : :: bincode :: de :: BorrowDecoder < '__de > >
    (decoder : & mut __D) ->core :: result :: Result < Self, :: bincode ::
    error :: DecodeError >
    {
        Ok(Self
        {
            fideid : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            name : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            country : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            sex : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            title : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            w_title : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            o_title : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            foa_title : :: bincode :: BorrowDecode :: borrow_decode(decoder)
            ?, rating : :: bincode :: BorrowDecode :: borrow_decode(decoder)
            ?, games : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            k : :: bincode :: BorrowDecode :: borrow_decode(decoder) ?,
            rapid_rating : :: bincode :: BorrowDecode ::
            borrow_decode(decoder) ?, rapid_games : :: bincode :: BorrowDecode
            :: borrow_decode(decoder) ?, rapid_k : :: bincode :: BorrowDecode
            :: borrow_decode(decoder) ?, blitz_rating : :: bincode ::
            BorrowDecode :: borrow_decode(decoder) ?, blitz_games : :: bincode
            :: BorrowDecode :: borrow_decode(decoder) ?, blitz_k : :: bincode
            :: BorrowDecode :: borrow_decode(decoder) ?, birthday : :: bincode
            :: BorrowDecode :: borrow_decode(decoder) ?, flag : :: bincode ::
            BorrowDecode :: borrow_decode(decoder) ?,
        })
    }
}