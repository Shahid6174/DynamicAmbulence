// basically  a Nothing to do route to test mongo deployement
import { connectToMongo } from '../../../lib/mongoConnect';
import mongoose from 'mongoose';

export async function GET() {
  await connectToMongo();
  const db = mongoose.connection.db;
  const stats = await db.stats();

  return Response.json({
    connected: true,
    db: stats.db,
  });
}